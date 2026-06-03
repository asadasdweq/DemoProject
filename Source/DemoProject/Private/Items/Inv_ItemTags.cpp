#include "Items\Inv_ItemTags.h"

namespace ItemTags
{
	namespace Equipment
	{
		namespace Weapons
		{
			UE_DEFINE_GAMEPLAY_TAG(Staff,"ItemTags.Equipment.Weapons.Staff");
		}

		namespace Cloaks
		{
			UE_DEFINE_GAMEPLAY_TAG(RedCloak,"ItemTags.Equipment.Cloaks.RedCloak");
		}

		namespace Mask
		{
			UE_DEFINE_GAMEPLAY_TAG(GrayMask,"ItemTags.Equipment.Mask.GrayMask");
		}
	}

	namespace Consumables
	{
		namespace Potions
		{
			namespace Red
			{
				UE_DEFINE_GAMEPLAY_TAG(Small,"ItemTags.Consumables.Potions.Red.Small");
				UE_DEFINE_GAMEPLAY_TAG(Large,"ItemTags.Consumables.Potions.Red.Large");
			}

			namespace Blue
			{
				UE_DEFINE_GAMEPLAY_TAG(Small,"ItemTags.Consumables.Potions.Blue.Small");
				UE_DEFINE_GAMEPLAY_TAG(Large,"ItemTags.Consumables.Potions.Blue.Large");
			}
		}
	}
}
