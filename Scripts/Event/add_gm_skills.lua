function OnEvent(Character)
    Character:SetNation(3)
	Character:AddSkillSlot(144, 1, -1)
	Character:AddSkillSlot(145, 1, -1)
	Character:AddSkillSlot(146, 1, -1)
	Character:AddSkillSlot(147, 1, -1)
    Character:DisconnectWorld()
end
