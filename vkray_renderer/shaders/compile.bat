%VULKAN_SDK%/Bin32/glslc.exe raygen.rgen -o raygen.rgen.spv --target-env=vulkan1.2
%VULKAN_SDK%/Bin32/glslc.exe closesthit.rchit -o closesthit.rchit.spv --target-env=vulkan1.2
%VULKAN_SDK%/Bin32/glslc.exe miss.rmiss -o miss.rmiss.spv --target-env=vulkan1.2
%VULKAN_SDK%/Bin32/glslc.exe shadow.rmiss -o shadow.rmiss.spv --target-env=vulkan1.2
pause
