// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class ARIller : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }

	public ARIller(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore", "ShaderCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        LoadOpenCV(Target);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}

    public bool LoadOpenCV(ReadOnlyTargetRules Target)
    {
        // Start OpenCV linking here!
        bool isLibrarySupported = false;

        // Create OpenCV Path 
        string OpenCVPath = Path.Combine(ThirdPartyPath, "OpenCV");

        // Get Library Path 
        string LibPath = "";
        //bool isdebug = Target.Configuration == UnrealTargetConfiguration.Debug && BuildConfiguration.bDebugBuildsActuallyUseDebugCRT;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            LibPath = Path.Combine(OpenCVPath, "Libraries", "Win64");
            isLibrarySupported = true;
        }
        else
        {
            string Err = string.Format("{0} dedicated server is made to depend on {1}. We want to avoid this, please correct module dependencies.", Target.Platform.ToString(), this.ToString()); System.Console.WriteLine(Err);
        }

        if (isLibrarySupported)
        {
            //Add Include path 
            PublicIncludePaths.AddRange(new string[] { Path.Combine(OpenCVPath, "Includes") });

            // Add Library Path 
            PublicLibraryPaths.Add(LibPath);

            //Add Static Libraries
            PublicAdditionalLibraries.Add("opencv_aruco400.lib");
            PublicAdditionalLibraries.Add("opencv_datasets400.lib");
            PublicAdditionalLibraries.Add("opencv_flann400.lib");
            PublicAdditionalLibraries.Add("opencv_imgproc400.lib");
            PublicAdditionalLibraries.Add("opencv_photo400.lib");
            PublicAdditionalLibraries.Add("opencv_stereo400.lib");
            PublicAdditionalLibraries.Add("opencv_tracking400.lib");
            PublicAdditionalLibraries.Add("opencv_xobjdetect400.lib");
            PublicAdditionalLibraries.Add("opencv_bgsegm400.lib");
            PublicAdditionalLibraries.Add("opencv_dnn_objdetect400.lib");
            PublicAdditionalLibraries.Add("opencv_fuzzy400.lib");
            PublicAdditionalLibraries.Add("opencv_line_descriptor400d.lib");
            PublicAdditionalLibraries.Add("opencv_plot400.lib");
            PublicAdditionalLibraries.Add("opencv_stitching400.lib");
            PublicAdditionalLibraries.Add("opencv_video400.lib");
            PublicAdditionalLibraries.Add("opencv_xphoto400.lib");
            PublicAdditionalLibraries.Add("opencv_bioinspired400.lib");
            PublicAdditionalLibraries.Add("opencv_dnn400.lib");
            PublicAdditionalLibraries.Add("opencv_hfs400.lib");
            PublicAdditionalLibraries.Add("opencv_ml400.lib");
            PublicAdditionalLibraries.Add("opencv_reg400.lib");
            PublicAdditionalLibraries.Add("opencv_structured_light400.lib");
            PublicAdditionalLibraries.Add("opencv_videoio400.lib");
            PublicAdditionalLibraries.Add("opencv_calib3d400.lib");
            PublicAdditionalLibraries.Add("opencv_dpm400.lib");
            PublicAdditionalLibraries.Add("opencv_highgui400.lib");
            PublicAdditionalLibraries.Add("opencv_objdetect400.lib");
            PublicAdditionalLibraries.Add("opencv_rgbd400.lib");
            PublicAdditionalLibraries.Add("opencv_superres400.lib");
            PublicAdditionalLibraries.Add("opencv_videostab400.lib");
            PublicAdditionalLibraries.Add("opencv_ccalib400.lib");
            PublicAdditionalLibraries.Add("opencv_face400.lib");
            PublicAdditionalLibraries.Add("opencv_img_hash400.lib");
            PublicAdditionalLibraries.Add("opencv_optflow400.lib");
            PublicAdditionalLibraries.Add("opencv_saliency400.lib");
            PublicAdditionalLibraries.Add("opencv_surface_matching400.lib");
            PublicAdditionalLibraries.Add("opencv_xfeatures2d400.lib");
            PublicAdditionalLibraries.Add("opencv_core400.lib");
            PublicAdditionalLibraries.Add("opencv_features2d400.lib");
            PublicAdditionalLibraries.Add("opencv_imgcodecs400.lib");
            PublicAdditionalLibraries.Add("opencv_phase_unwrapping400.lib");
            PublicAdditionalLibraries.Add("opencv_shape400.lib");
            PublicAdditionalLibraries.Add("opencv_text400.lib");
            PublicAdditionalLibraries.Add("opencv_ximgproc400.lib");



            //debug
            PublicAdditionalLibraries.Add("opencv_aruco400d.lib");
            PublicAdditionalLibraries.Add("opencv_datasets400d.lib");
            PublicAdditionalLibraries.Add("opencv_flann400d.lib");
            PublicAdditionalLibraries.Add("opencv_imgproc400d.lib");
            PublicAdditionalLibraries.Add("opencv_photo400d.lib");
            PublicAdditionalLibraries.Add("opencv_stereo400d.lib");
            PublicAdditionalLibraries.Add("opencv_tracking400d.lib");
            PublicAdditionalLibraries.Add("opencv_xobjdetect400d.lib");
            PublicAdditionalLibraries.Add("opencv_bgsegm400d.lib");
            PublicAdditionalLibraries.Add("opencv_dnn_objdetect400d.lib");
            PublicAdditionalLibraries.Add("opencv_fuzzy400d.lib");
            PublicAdditionalLibraries.Add("opencv_line_descriptor400d.lib");
            PublicAdditionalLibraries.Add("opencv_plot400d.lib");
            PublicAdditionalLibraries.Add("opencv_stitching400d.lib");
            PublicAdditionalLibraries.Add("opencv_video400d.lib");
            PublicAdditionalLibraries.Add("opencv_xphoto400d.lib");
            PublicAdditionalLibraries.Add("opencv_bioinspired400d.lib");
            PublicAdditionalLibraries.Add("opencv_dnn400d.lib");
            PublicAdditionalLibraries.Add("opencv_hfs400d.lib");
            PublicAdditionalLibraries.Add("opencv_ml400d.lib");
            PublicAdditionalLibraries.Add("opencv_reg400d.lib");
            PublicAdditionalLibraries.Add("opencv_structured_light400d.lib");
            PublicAdditionalLibraries.Add("opencv_videoio400d.lib");
            PublicAdditionalLibraries.Add("opencv_calib3d400d.lib");
            PublicAdditionalLibraries.Add("opencv_dpm400d.lib");
            PublicAdditionalLibraries.Add("opencv_highgui400d.lib");
            PublicAdditionalLibraries.Add("opencv_objdetect400d.lib");
            PublicAdditionalLibraries.Add("opencv_rgbd400d.lib");
            PublicAdditionalLibraries.Add("opencv_superres400d.lib");
            PublicAdditionalLibraries.Add("opencv_videostab400d.lib");
            PublicAdditionalLibraries.Add("opencv_ccalib400d.lib");
            PublicAdditionalLibraries.Add("opencv_face400d.lib");
            PublicAdditionalLibraries.Add("opencv_img_hash400d.lib");
            PublicAdditionalLibraries.Add("opencv_optflow400d.lib");
            PublicAdditionalLibraries.Add("opencv_saliency400d.lib");
            PublicAdditionalLibraries.Add("opencv_surface_matching400d.lib");
            PublicAdditionalLibraries.Add("opencv_xfeatures2d400d.lib");
            PublicAdditionalLibraries.Add("opencv_core400d.lib");
            PublicAdditionalLibraries.Add("opencv_features2d400d.lib");
            PublicAdditionalLibraries.Add("opencv_imgcodecs400d.lib");
            PublicAdditionalLibraries.Add("opencv_phase_unwrapping400d.lib");
            PublicAdditionalLibraries.Add("opencv_shape400d.lib");
            PublicAdditionalLibraries.Add("opencv_text400d.lib");
            PublicAdditionalLibraries.Add("opencv_ximgproc400d.lib");


            //Add Dynamic Libraries
            //release
            PublicDelayLoadDLLs.Add("opencv_aruco400.dll");
            PublicDelayLoadDLLs.Add("opencv_datasets400.dll");
            PublicDelayLoadDLLs.Add("opencv_flann400.dll");
            PublicDelayLoadDLLs.Add("opencv_imgproc400.dll");
            PublicDelayLoadDLLs.Add("opencv_photo400.dll");
            PublicDelayLoadDLLs.Add("opencv_stereo400.dll");
            PublicDelayLoadDLLs.Add("opencv_tracking400.dll");
            PublicDelayLoadDLLs.Add("opencv_xobjdetect400.dll");
            PublicDelayLoadDLLs.Add("opencv_bgsegm400.dll");
            PublicDelayLoadDLLs.Add("opencv_dnn_objdetect400.dll");
            PublicDelayLoadDLLs.Add("opencv_fuzzy400.dll");
            PublicDelayLoadDLLs.Add("opencv_line_descriptor400d.dll");
            PublicDelayLoadDLLs.Add("opencv_plot400.dll");
            PublicDelayLoadDLLs.Add("opencv_stitching400.dll");
            PublicDelayLoadDLLs.Add("opencv_video400.dll");
            PublicDelayLoadDLLs.Add("opencv_xphoto400.dll");
            PublicDelayLoadDLLs.Add("opencv_bioinspired400.dll");
            PublicDelayLoadDLLs.Add("opencv_dnn400.dll");
            PublicDelayLoadDLLs.Add("opencv_hfs400.dll");
            PublicDelayLoadDLLs.Add("opencv_ml400.dll");
            PublicDelayLoadDLLs.Add("opencv_reg400.dll");
            PublicDelayLoadDLLs.Add("opencv_structured_light400.dll");
            PublicDelayLoadDLLs.Add("opencv_videoio400.dll");
            PublicDelayLoadDLLs.Add("opencv_cadll3d400.dll");
            PublicDelayLoadDLLs.Add("opencv_dpm400.dll");
            PublicDelayLoadDLLs.Add("opencv_highgui400.dll");
            PublicDelayLoadDLLs.Add("opencv_objdetect400.dll");
            PublicDelayLoadDLLs.Add("opencv_rgbd400.dll");
            PublicDelayLoadDLLs.Add("opencv_superres400.dll");
            PublicDelayLoadDLLs.Add("opencv_videostab400.dll");
            PublicDelayLoadDLLs.Add("opencv_ccadll400.dll");
            PublicDelayLoadDLLs.Add("opencv_face400.dll");
            PublicDelayLoadDLLs.Add("opencv_img_hash400.dll");
            PublicDelayLoadDLLs.Add("opencv_optflow400.dll");
            PublicDelayLoadDLLs.Add("opencv_saliency400.dll");
            PublicDelayLoadDLLs.Add("opencv_surface_matching400.dll");
            PublicDelayLoadDLLs.Add("opencv_xfeatures2d400.dll");
            PublicDelayLoadDLLs.Add("opencv_core400.dll");
            PublicDelayLoadDLLs.Add("opencv_features2d400.dll");
            PublicDelayLoadDLLs.Add("opencv_imgcodecs400.dll");
            PublicDelayLoadDLLs.Add("opencv_phase_unwrapping400.dll");
            PublicDelayLoadDLLs.Add("opencv_shape400.dll");
            PublicDelayLoadDLLs.Add("opencv_text400.dll");
            PublicDelayLoadDLLs.Add("opencv_ximgproc400.dll");

            //debug
            PublicDelayLoadDLLs.Add("opencv_aruco400d.dll");
            PublicDelayLoadDLLs.Add("opencv_datasets400d.dll");
            PublicDelayLoadDLLs.Add("opencv_flann400d.dll");
            PublicDelayLoadDLLs.Add("opencv_imgproc400d.dll");
            PublicDelayLoadDLLs.Add("opencv_photo400d.dll");
            PublicDelayLoadDLLs.Add("opencv_stereo400d.dll");
            PublicDelayLoadDLLs.Add("opencv_tracking400d.dll");
            PublicDelayLoadDLLs.Add("opencv_xobjdetect400d.dll");
            PublicDelayLoadDLLs.Add("opencv_bgsegm400d.dll");
            PublicDelayLoadDLLs.Add("opencv_dnn_objdetect400d.dll");
            PublicDelayLoadDLLs.Add("opencv_fuzzy400d.dll");
            PublicDelayLoadDLLs.Add("opencv_line_descriptor400d.dll");
            PublicDelayLoadDLLs.Add("opencv_plot400d.dll");
            PublicDelayLoadDLLs.Add("opencv_stitching400d.dll");
            PublicDelayLoadDLLs.Add("opencv_video400d.dll");
            PublicDelayLoadDLLs.Add("opencv_xphoto400d.dll");
            PublicDelayLoadDLLs.Add("opencv_bioinspired400d.dll");
            PublicDelayLoadDLLs.Add("opencv_dnn400d.dll");
            PublicDelayLoadDLLs.Add("opencv_hfs400d.dll");
            PublicDelayLoadDLLs.Add("opencv_ml400d.dll");
            PublicDelayLoadDLLs.Add("opencv_reg400d.dll");
            PublicDelayLoadDLLs.Add("opencv_structured_light400d.dll");
            PublicDelayLoadDLLs.Add("opencv_videoio400d.dll");
            PublicDelayLoadDLLs.Add("opencv_cadll3d400d.dll");
            PublicDelayLoadDLLs.Add("opencv_dpm400d.dll");
            PublicDelayLoadDLLs.Add("opencv_highgui400d.dll");
            PublicDelayLoadDLLs.Add("opencv_objdetect400d.dll");
            PublicDelayLoadDLLs.Add("opencv_rgbd400d.dll");
            PublicDelayLoadDLLs.Add("opencv_superres400d.dll");
            PublicDelayLoadDLLs.Add("opencv_videostab400d.dll");
            PublicDelayLoadDLLs.Add("opencv_ccadll400d.dll");
            PublicDelayLoadDLLs.Add("opencv_face400d.dll");
            PublicDelayLoadDLLs.Add("opencv_img_hash400d.dll");
            PublicDelayLoadDLLs.Add("opencv_optflow400d.dll");
            PublicDelayLoadDLLs.Add("opencv_saliency400d.dll");
            PublicDelayLoadDLLs.Add("opencv_surface_matching400d.dll");
            PublicDelayLoadDLLs.Add("opencv_xfeatures2d400d.dll");
            PublicDelayLoadDLLs.Add("opencv_core400d.dll");
            PublicDelayLoadDLLs.Add("opencv_features2d400d.dll");
            PublicDelayLoadDLLs.Add("opencv_imgcodecs400d.dll");
            PublicDelayLoadDLLs.Add("opencv_phase_unwrapping400d.dll");
            PublicDelayLoadDLLs.Add("opencv_shape400d.dll");
            PublicDelayLoadDLLs.Add("opencv_text400d.dll");
            PublicDelayLoadDLLs.Add("opencv_ximgproc400d.dll");

        }

        Definitions.Add(string.Format("WITH_OPENCV_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;

    }
}
