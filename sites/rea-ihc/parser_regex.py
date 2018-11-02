# -*- coding: utf-8 -*-

import re

re_identificador = r"[_a-zA-Z][_a-zA-Z0-9]*"
re_funcao = re_identificador + r"\s(?P<funcao>" + re_identificador + r")\(.*\);"
re_instrucao = r"\d+\t.*"
re_breakpoint = r"^Breakpoint\s\d+, (?P<funcao>" + re_identificador + r")"
re_retorno = r"^(?P<funcao>" + re_identificador + r")"
re_frame = r"(?P<var>^" + re_identificador + r" = .+?\n)+"
re_char = r"\d+\s(?P<valor>\'.+\')"
re_alloc = r"(m|c|re)alloc\s*\((?P<operadores>.*)\)"

identificador_pattern = re.compile(re_identificador)
instrucao_pattern = re.compile(re_instrucao)
breakpoint_pattern = re.compile(re_breakpoint, re.MULTILINE)
retorno_pattern = re.compile(re_retorno, re.MULTILINE)
frames_pattern = re.compile(re_frame, re.DOTALL | re.MULTILINE)
char_pattern = re.compile(re_char)
alloc_pattern = re.compile(re_alloc)
