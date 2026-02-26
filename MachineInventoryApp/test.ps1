[xml] $xml = Get-Content "source.xml"
$project = $xml.project

foreach ($file in $project.sources.file) {
    $in = $file.path -replace "/", "\"
    $out = $in -replace "\\", "_" -replace ".cpp", ".o"

    echo ("Entrada: " + $project.sources.root + "\$in  Saída: $out")
}