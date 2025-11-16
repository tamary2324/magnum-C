class Translator:

    def __init__(self, file_path):
        self.file_path = file_path
        self.processes = {
            "MAKE": [self.make, 3],
            "SET": [self.set, 2],
            "COPYMAG": [self.copy_mag, 2],
            "FUNC": [self.set, 2]
        }
        self.numbers = [str(i) for i in range(10)]
        self.letters = [char for char in "abcdefghijklmnopqrstuvwxyz"]
        self.operators = {
            "+": "add",
            "-": "sub"
        }
        self.stored_vars = {}
        self.stored_funcs = {}
        self.translation = []

    def split_lines(self, text):
        depth = 0
        last_end = 0
        split_string = []
        for idx, char in enumerate(text):
            if char == ";":
                if depth == 0:
                    split_string.append(text[last_end:idx])
                    last_end = idx+1
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
        split_string.append(text[last_end:])
        return split_string

    def split(self, string):
        depth = 0
        last_end = 0
        split_string = []
        for idx, char in enumerate(string):
            if char == " ":
                if depth == 0:
                    split_string.append(string[last_end:idx])
                    last_end = idx+1
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
        split_string.append(string[last_end:])
        return split_string

    def write(self, line):
        self.translation.append((self.depth * " ") + line)

    def translate_expr_int(self, expr):
        expr = expr[1:-1]
        split_expr = self.split(expr)
        if len(split_expr) == 1:
            if split_expr[0][0] in self.numbers:
                try:
                    int(split_expr[0])
                except ValueError:
                    raise Exception("pymagc translator : Expected int value")
            else:
                if not (split_expr[0] in self.stored_vars):
                    raise Exception("pymagc translator : Unknown variable")
                if self.stored_vars[split_expr[0]] != "int":
                    raise Exception("pymagc translator : Expected int variable")
            expr_to_write = split_expr[0]
        elif len(split_expr) == 3:
            if split_expr[1] in self.operators:
                expr_to_write = (
                    self.translate_expr_int(split_expr[0])
                    + " "
                    + split_expr[1]
                    + " "
                    + self.translate_expr_int(split_expr[2])
                )
            else:
                raise Exception("pymagc translator : Unknown operator")
        elif split_expr[0] in self.stored_funcs:
            if self.stored_funcs[split_expr[0]] != "int":
                raise Exception("pymagc translator : Function called is not of type int")
            pass
        else:
            raise Exception("pymagc translator : Int expression was not able to be translated")
        return expr_to_write

    def translate_expr_float(self, expr):
        expr = expr[1:-1]
        split_expr = self.split(expr)
        if len(split_expr) == 1:
            if split_expr[0][0] == "." or split_expr[0][0] in self.numbers:
                try:
                    float(split_expr[0])
                except ValueError:
                    raise Exception("pymagc translator : Expected float value")
            else:
                if not (split_expr[0] in self.stored_vars):
                    raise Exception("pymagc translator : Unknown variable")
                if self.stored_vars[split_expr[0]] != "float":
                    raise Exception("pymagc translator : Expected float variable")
            expr_to_write = split_expr[0]
        elif len(split_expr) == 3:
            if split_expr[1] in self.operators:
                expr_to_write = (
                    self.translate_expr_float(split_expr[0])
                    + " "
                    + split_expr[1]
                    + " "
                    + self.translate_expr_float(split_expr[2])
                )
            else:
                raise Exception("pymagc translator : Unknown operator")
        elif split_expr[0] in self.stored_funcs:
            if self.stored_funcs[split_expr[0]] != "float":
                raise Exception("pymagc translator : Function called is not of type float")
            pass
        else:
            raise Exception("pymagc translator : Float expression was not able to be translated")
        return expr_to_write

    def translate_expr_mag(self, expr, expression_depth=0):
        expr = expr[1:-1]
        split_expr = self.split(expr)
        if split_expr[0] == "MAG":
            if split_expr[1] == "int":
                expr_to_write = (
                    "to_magnum_from_int("
                    + self.translate_expr_int(split_expr[2])
                    + ")"
                )
            elif split_expr[1] == "float":
                expr_to_write = (
                    "to_magnum_from_double("
                    + self.translate_expr_float(split_expr[2])
                    + ")"
                )
            else:
                raise Exception("pymagc translator : Type cannot be turned into magnum")
        elif len(split_expr) == 3:
            if split_expr[1] in self.operators:
                expr_to_write = (
                    self.translate_expr_mag(split_expr[0], expression_depth=expression_depth+1)
                    + " "
                    + split_expr[1]
                    + " "
                    + self.translate_expr_mag(split_expr[2], expression_depth=expression_depth+1)
                )
            else:
                raise Exception("pymagc translator : Unknown operator")
        elif split_expr[0] in self.stored_funcs:
            if self.stored_funcs[split_expr[0]] != "mag":
                raise Exception("pymagc translator : Function called is not of type mag")
            pass
        else:
            raise Exception("pymagc translator : Magnum expression was not able to be translated")
        return expr_to_write

    def make(self, args):  # args = [type, name, expr]
        if not (args[1][0] in self.letters):
            raise Exception("pymagc translator : Invalid variable name")
        if args[1] in self.stored_vars:
            raise Exception("pymagc translator : Variable already exists")
        if args[0] == "int":
            line_to_write = f"int {args[1]} = "
            expr = self.translate_expr_int(args[2])
        elif args[0] == "float":
            line_to_write = f"float {args[1]} = "
            expr = self.translate_expr_float(args[2])
        elif args[0] == "mag":
            line_to_write = f"struct magnum * {args[1]} = "
            expr = self.translate_expr_mag(args[2])
        else:
            raise Exception("pymagc translator : Unsupported type for make")
        self.stored_vars.update({args[1]: args[0]})
        line_to_write += expr + ";"
        self.write(line_to_write)

    def set(self, args):  # args = [name, expr]
        if not (args[0] in self.stored_vars):
            raise Exception("pymagc translator : Unknown variable")
        if self.stored_vars[args[0]] == "int":
            expr = self.translate_expr_int(args[1])
        elif self.stored_vars[args[0]] == "float":
            expr = self.translate_expr_float(args[1])
        elif self.stored_vars[args[0]] == "mag":
            pass
            '''
            Delete what was previously at the mag address and create a new mag with the wanted value
             -> however, need to get the type to know what convert function to call
            
            Free_magnum(m1)
            m1 = to_magnum_from_int(3)
            
            Also -> need to check that the final expression does not contain m1

            ALTERNATIVE SOLUTION -> do not allow set for magnums???
            '''
        else:
            raise Exception("pymagc translator : Unsupported type for set")
        line_to_write = f"{args[0]} = " + expr + ";"
        self.write(line_to_write)

    def copy_mag(self, args):  # args = [magnum to be copied into, magnum to be copied from]
        if len(args) != 2:
            raise Exception("pymagc translator : Incorrect amount of arguments")
        for variable in args:
            if not (variable in self.stored_vars):
                raise Exception("pymagc translator : Unknown variable")
            if self.stored_vars[variable] != "mag":
                raise Exception("pymagc translator : Variable is not a magnum")
        line_to_write = f"copy({args[0]}, {args[1]})"
        self.write(line_to_write)

    def func(self, args):  # args = [return type, name, func args (type name)]
        if not (args[1][0] in self.letters):
            raise Exception("pymagc translator : Invalid function name")
        if args[1] in self.stored_vars:
            raise Exception("pymagc translator : Function already exists")
        if self.in_function:
            raise Exception("pymagc translator : Cannot define function in function")
        if self.depth != 0:
            raise Exception("pymagc translator : Can only create functions at a depth of 0")
        if args[0] == "mag":
            line_to_write = "struct magnum * " + args[0]
        elif (args[0] == "int") or (args[0] == "float"):
            line_to_write = args[0] + " " + args[1]
        else:
            raise Exception("pymagc translator : Unknown type for function definition")
        self.write(line_to_write)
        self.in_function = True
        self.depth += 1

    def func_return(self, args):
        pass

    def done(self, args):
        if self.depth == 0:
            raise Exception("pymagc translator : Nothing to be done with... o_O")
        if self.in_function and (self.depth == 1):
            self.depth -= 1
            self.in_function = False
        else:
            self.depth -= 1
        line_to_write = "}"
        self.write(line_to_write)

    def self_op(self, args):
        pass

    def translate_line(self, line):
        print("line", line)
        words = self.split(line)
        while words[0] == "":
            words.pop(0)
        print("words", words)
        if words[0] in self.processes:
            n_of_args_needed = self.processes[words[0]][1]
            if len(words) - 1 < n_of_args_needed:
                raise Exception("pymagc translator : Missing arguments")
            else:
                args = words[1:n_of_args_needed+1]
                self.processes[words[0]][0](args)
        else:
            pass

    def translate(self):
        with open(self.file_path, "r") as file:
            str_file = file.read()
            file.close()
        lines = self.split_lines(str_file.replace("\n", ""))
        self.depth = 0
        self.in_function = False
        for line in lines:
            self.translate_line(line)
        if self.depth != 0:
            raise Exception("pymagc translator : Depth is not 0")
        return self.translation


'''
#### NOTES

## general
; at the end of every line except last (can be on last but redundant)

## script
script -> several lines
defining a script :
SCRIPT {name};
...
END

## line
line types:
MAKE {var name} {type: int / mag} {value};
SET {var name} {expr};
CYCLE {iterator} {start} {stop} {script name};
WHILE {condition} {script name};
FUNC {func name} {type}

## expr
expr -> what can be found in the def of a variable

CYCLE i 0 n
    SET ...
    ...
DONE

FUNC a ((type name) (type name) (type name) ... ) (output type)
...
DONE






Make capable of writing data into a file???
'''
