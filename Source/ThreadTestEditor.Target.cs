// Copyright (c) 2025 Xist.GG LLC

using UnrealBuildTool;
using System.Collections.Generic;

public class ThreadTestEditorTarget : TargetRules
{
	public ThreadTestEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("ThreadTest");
	}
}
