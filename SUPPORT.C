#include	<goose/goose.h>
#include	"include/support.h"

sprite_info_t *beginobj_with_id(image_info_t *ii, float x, float y, float z, int tid, int id, int scale_overide)
{
	sprite_info_t	*si;

	if(scale_overide)
	{
		is_low_res ^= 1;
	}
	si = beginobj(ii, x, y, z, tid);
	if(scale_overide)
	{
		is_low_res ^= 1;
	}
	if(!si)
	{
		return(si);
	}
	si->id = id;
	return(si);
}

