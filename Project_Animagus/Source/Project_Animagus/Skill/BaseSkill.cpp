// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSkill.h"

void UBaseSkill::ActiveSkill()
{
}

void UBaseSkill::DeactiveSkill()
{
}

void UBaseSkill::StartCoolDown()
{
    is_cooldown = true;
}

void UBaseSkill::EndCoolDown()
{
    is_cooldown = false;
}
