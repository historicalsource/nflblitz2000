#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <goose/jmalloc.h>

#define FILE_LEN		32		/* stored filename limit    */
#define BTABLE_ALLOC_SIZE	8		/* allocate X at a time     */
#define FENCE_BYTES		4		/* bytes in each fence      */
#define FENCE_VAL		0xAA		/* fence byte value         */

typedef struct t_mblock {
  char filespec[FILE_LEN];
  int line;
  size_t size;
  void *ptr;
} mblock;

#ifdef MALLOC_DEBUG
unsigned int table_entries = 0;
unsigned int used_entries = 0;
mblock *mblock_table;

void *jmalloc( size_t size, char *file, int line )
{
void *mem;
char *fence;
mblock *new_block;
unsigned int i;

  check_fences();

  if( used_entries == table_entries ) {
    /* make the table bigger */
    table_entries += BTABLE_ALLOC_SIZE;
    new_block = (mblock *)realloc( (void *)mblock_table,
                ( table_entries * sizeof( mblock )) );
    
    /* return NULL if we can't */
    if( new_block == NULL ) {
      table_entries -= BTABLE_ALLOC_SIZE;
      return NULL;
    }
    
    mblock_table = new_block;
  }
  
  /* allocate the block, plus room for fences on either side */
  mem = malloc( size + 2 * FENCE_BYTES );

  if( mem == NULL ) {
    fprintf( stderr, "Failure allocating pointer at %s:%d\n", file, line );
    return NULL;
  }

  /* install the fences */
  fence = (char *)mem;
  for( i = 0; i < FENCE_BYTES; i++ )
    fence[i] = FENCE_VAL;

  fence = (char *)mem + size + FENCE_BYTES;
  for( i = 0; i < FENCE_BYTES; i++ )
    fence[i] = FENCE_VAL;

  /* prepare the mblock entry */
  strncpy( mblock_table[used_entries].filespec, file, FILE_LEN );
  mblock_table[used_entries].line = line;
  mblock_table[used_entries].ptr = mem;
  mblock_table[used_entries].size = size;
  used_entries++;

  /* point mem to the byte AFTER the first fence */
  fence = (char *)mem + FENCE_BYTES;
  mem = (void *)fence;

  return mem;
}


void jfree( void *ptr, char *file, int line )
{
unsigned int i;
void *search_ptr;

  check_fences();

  search_ptr = (void *)((char *)ptr - FENCE_BYTES );

  /* search our list for this pointer */
  for( i = 0; i < used_entries; i++ ) {
    if( mblock_table[i].ptr == search_ptr )
      break;
  }

  if( i == used_entries ) {
    fprintf( stderr, "Attempt to free unallocated pointer %#010x at %s:%d.\n",
             (int)ptr, file, line );
    return;
  }

  /* okay, it's a real pointer.  first, free the block */
  free( search_ptr );

  /* shuffle the other pointers down into place */
  for( i++; i < used_entries; i++ )
    mblock_table[i-1] = mblock_table[i];

  used_entries--;
  
}


void dump_alloclist( void )
{
unsigned int i;

  check_fences();

  fprintf( stderr, "allocated at      ptr          bytes\n" );
  fprintf( stderr, "--------------------------------------------------\n" );
  for( i = 0; i < used_entries; i++ ) {
    fprintf( stderr, "%s:%-*d %#010x   %lu\n", mblock_table[i].filespec,
             (int)(16 - strlen( mblock_table[i].filespec )), 
             mblock_table[i].line, (unsigned long int)mblock_table[i].ptr,
             mblock_table[i].size );
  }
}


void check_fences( void )
{
	unsigned char	*fence;
	unsigned int	block, i;

	check_heap();
	check_free();

	for( block = 0; block < used_entries; block++ )
	{
		fence = (unsigned char *)mblock_table[block].ptr;
		for( i = 0; i < FENCE_BYTES; i++ )
		{
			if( fence[i] != FENCE_VAL )
			{	/* fence corrupted.  throw a fit */
				fprintf( stderr, "WARNING: Leading fence damaged: %#010x %s:%d si"
					"ze: %lu\n", (unsigned long int)mblock_table[block].ptr,
					mblock_table[block].filespec, mblock_table[block].line,
					mblock_table[block].size );
			}
		}

		fence = (unsigned char *)mblock_table[block].ptr + mblock_table[block].size
			+ FENCE_BYTES;

		for( i = 0; i < FENCE_BYTES; i++ )
		{
			if( fence[i] != FENCE_VAL )
			{
				/* fence corrupted.  throw a fit */
				fprintf( stderr, "WARNING: Trailing fence damaged: %#010x %s:%d si"
					"ze: %lu\n", (unsigned long int)mblock_table[block].ptr,
					mblock_table[block].filespec, mblock_table[block].line,
					mblock_table[block].size );
			}
		}
	}
}

void dump_block( unsigned int block )
{
unsigned int i;
char *array;

  check_fences();

  if( block >= used_entries ) {
    fprintf( stderr, "Shit. %d %d\n", block, used_entries );
    return;
  }

  array = (char *)mblock_table[block].ptr;

  for( i = 0; i < mblock_table[block].size + 2 * FENCE_BYTES; i++ )
    printf( "%d ", array[i] );

  printf( "\n" );
}

void *jrealloc( void *ptr, size_t size, char *file, int line )
{
unsigned int i, j;
void *search_ptr, *new_block;
char *fence;

  /* if it's a NULL pointer, call jmalloc instead */
  if( ptr == NULL )
    return( jmalloc( size, file, line ));

  check_fences();

  /* search our list for this pointer */
  search_ptr = (char *)ptr - FENCE_BYTES;
  for( i = 0; i < used_entries; i++ ) {
    if( mblock_table[i].ptr == search_ptr )
      break;
  }

  if( i == used_entries ) {
    fprintf( stderr, "Attempt to realloc unallocated pointer %#010x at %s:%d.\n",
             (int)ptr, file, line );
    return NULL;
  }

  /* try to reallocate */
  new_block = realloc( search_ptr, size + 2 * FENCE_BYTES );

  if( new_block == NULL )
    return NULL;

  /* whee!  we've reallocated.  relocate the trailing fence */
  fence = (char *)new_block;
  for( j = 0; j < FENCE_BYTES; j++ ) {
    /* zero the old, unless the block got smaller */
    if( size > mblock_table[i].size )
      fence[j+mblock_table[i].size+FENCE_BYTES] = 0x00;
    /* set the new */
    fence[j+size+FENCE_BYTES] = FENCE_VAL;
  }

  /* update the mblock_table */
  mblock_table[i].size = size;
  mblock_table[i].ptr = new_block;

  fence = (char *)new_block + FENCE_BYTES;
  return((void *)fence ); 
}
#endif
