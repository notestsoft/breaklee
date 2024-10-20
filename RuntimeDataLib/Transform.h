RUNTIME_DATA_FILE_BEGIN(Transform.enc)

RUNTIME_DATA_TYPE_BEGIN(Transform, "Transform.Transform_mob.Item_msg")
	RUNTIME_DATA_PROPERTY(Int32, TransformIndex, "id")
	RUNTIME_DATA_PROPERTY(Int32, MobSpeciesIndex, "mob_id")
	RUNTIME_DATA_PROPERTY(Int32, SocialSkillIndex, "s_id")
	RUNTIME_DATA_PROPERTY(Int32, AcquireIndex, "acquire")
RUNTIME_DATA_TYPE_END(Transform)
RUNTIME_DATA_TYPE_INDEX(Transform, Int32, TransformIndex)

RUNTIME_DATA_FILE_END
