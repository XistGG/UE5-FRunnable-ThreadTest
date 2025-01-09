// Copyright (c) 2025 Xist.GG LLC

using UnrealBuildTool;
using System.Collections.Generic;

public class ThreadTestTarget : TargetRules
{
	public ThreadTestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("ThreadTest");
	}
}
