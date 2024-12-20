param (
    [Parameter(Mandatory=$false)]
    [Switch]$useDebug,
    [Parameter(Mandatory=$false)]
    [Switch]$log
)

& ./build.ps1
if ($useDebug -eq $true) {
    & adb push build/debug/libconfig-utils_test.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/early_mods/libconfig-utils_test.so
} else {
    & adb push build/libconfig-utils_test.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/early_mods/libconfig-utils_test.so
}

& adb shell am force-stop com.beatgames.beatsaber
& adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
if ($log.IsPresent) {
    & ./log.ps1
}
