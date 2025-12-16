$INCLUDE_PATH = "./include"
$LIB_PATH     = "./lib"
$SRC_PATH     = "./src"

$SOURCES      = @(
  "main.c"

  "${src_path}/code-generator.c"
  "${src_path}/lexer.c"
  "${src_path}/parser.c"
  "${src_path}/utils.c",

  "${LIB_PATH}/c_map.c"
)

$CCOMPILER = "clang"
$CFLAGS    = "-Wextra -O2 "

$command = "$CCOMPILER $CFLAGS"

foreach($FILE in $SOURCES)
{
  $command += "-I$INCLUDE_PATH $FILE ";
}

$command += "-o main && ./main.exe"

Invoke-Expression $command