#pragma once
#include"NativeGameplayTags.h"

namespace ItemTags
{
	namespace Equipment
	{
		namespace Weapons
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Staff)
		}

		namespace Cloaks
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(RedCloak)
		}

		namespace Masks
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(GrayMask)
		}
	}

	namespace Consumables
	{
		namespace Potions
		{
			namespace Red
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Small)
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Large)
			}
			namespace Blue
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Small)
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Large)
			}
		}
	}
}
