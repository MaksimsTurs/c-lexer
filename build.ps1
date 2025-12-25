$INCLUDE_PATH = "./include"
$LIB_PATH     = "./lib"
$SRC_PATH     = "./src"
$BUILD_TARGET = "debug"
$CCOMPILER    = "clang"
$CFLAGS       = "-Wall -Wextra -O2 "
$SOURCES      = @(
  "main.c"

  "${src_path}/lexer.c"
  # "${src_path}/parser.c"
  "${src_path}/nasm.c"
  "${src_path}/module.c"

  "${LIB_PATH}/c_map.c"
)

function Create-Compile-Command($target)
{
  $logTarget = Get-Log-Flags-From-Target($target)
  $command = "$CCOMPILER $logTarget $CFLAGS"
  
  foreach($FILE in $SOURCES)
  {
    $command += "-I$INCLUDE_PATH $FILE ";
  }
  
  $command += "-o main"

  return $command
}

function Get-Log-Flags-From-Target($target)
{
  switch($target)
  {
    "debug" {
      return "-DASTRO_DEBUG"
    }
    "trace" {
      return "-DASTRO_TRACE"
    }
    "destr" {
      return "-DASTRO_DESTR"
    }
  }
}

$command = Create-Compile-Command($BUILD_TARGET)

Invoke-Expression $command