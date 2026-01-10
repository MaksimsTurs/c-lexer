$INCLUDE_PATH = "./include"
$SRC_PATH     = "./src"

$CCOMPILER    = "clang"
$CFLAGS       = "-std=c11 "

$SOURCES      = @(
  "main.c"

  "${src_path}/expr.c"
  "${src_path}/scope.c"
  "${src_path}/lexer.c"
  "${src_path}/nasm.c"
)

function NewCompileCommand($target)
{
  $command = "$CCOMPILER $CFLAGS"
  
  foreach($FILE in $SOURCES)
  {
    $command += "-I$INCLUDE_PATH $FILE ";
  }
  
  $command += "-o main"

  return $command
}

$command = NewCompileCommand($BUILD_TARGET)

Invoke-Expression $command