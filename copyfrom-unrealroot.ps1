# Copies the Plugins\Marketplace folder under $ueroot to here.
# $ueroot is the Unreal code root folder (the one which contains e.g. Engine and LICENSE.md)
param (
	[Parameter(Mandatory = $true)] 
	[string]$ueroot
)

$pluginName = "Prefabricator"

$pluginRoot = join-path $ueroot "Engine\Plugins\Marketplace\$pluginName"

$files = git ls-tree -r --name-only embark
$files | foreach {
	$dest = $_
	if (test-path $dest -PathType Leaf) {
		$src = (join-path $pluginRoot $dest)
		copy-item $src $dest
	}
}
