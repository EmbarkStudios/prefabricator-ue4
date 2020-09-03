# Copies the workspace to the Plugins\Marketplace folder under $ueroot.
# $ueroot is the Unreal code root folder (the one which contains e.g. Engine and LICENSE.md)
param (
	[Parameter(Mandatory = $true)] 
	[string]$ueroot
)

$pluginRoot = join-path $ueroot "Engine\Plugins\Marketplace\Prefabricator"
if (test-path $pluginRoot) {
	remove-item $pluginRoot -recurse -force
}
new-item $pluginRoot -ItemType Directory
$files = git ls-tree -r --name-only embark
$files | foreach {
	$src = $_
	if (test-path $src -PathType Leaf) {
		$dest = (join-path $pluginRoot $src)
		(New-Item -Path (Split-Path -Path $dest) -Type Directory -ErrorAction SilentlyContinue)
		copy-item $src $dest
	}
}


$sha = (git rev-parse HEAD)
$remote = (git remote get-url --push origin)

$pluginOrigin = New-Item -ItemType File -Path (join-path $pluginRoot "PLUGIN_ORIGIN.txt")
$remote >> $pluginOrigin
"commit $sha" >> $pluginOrigin

