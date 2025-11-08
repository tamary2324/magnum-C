class Translator:

    def __init__(self, file_path):
        self.file_path = file_path
        self.processes = {
            "MAKE": [self.make, 3],
            "SET": [self.set, 2]
        }
        self.numbers = [str(i) for i in range(10)]
        self.letters = [char for char in 'abcdefghijklmnopqrstuvwxyz']
        self.operators = {
            "+": "add",
            "-": "sub"
        }
        self.stored_vars = {}
        self.translation = []

    def split_lines(self, text):
        depth = 0
        last_end = 0
        split_string = []
        for idx, char in enumerate(text):
            if char == ';':
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
        self.translation.append(line)

    def translate_expr(self, expr, expr_type):
        if expr_type == "int":
            return self.translate_expr_int(expr)

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
                if split_expr[0] in self.stored_vars:
                    if self.stored_vars[split_expr[0]] != "int":
                        raise Exception("pymagc translator : Expected int variable")
                else:
                    raise Exception("pymagc translator : Unknown variable")
            expr_to_write = split_expr[0]
        elif split_expr[0] == "FUNC":
            pass
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
                if split_expr[0] in self.stored_vars:
                    if self.stored_vars[split_expr[0]] != "float":
                        raise Exception("pymagc translator : Expected float variable")
                else:
                    raise Exception("pymagc translator : Unknown variable")
            expr_to_write = split_expr[0]
        elif split_expr[0] == "FUNC":
            pass
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
        return expr_to_write

    def translate_expr_mag(self, expr):
        expr = expr[1:-1]
        split_expr = self.split(expr)
        if split_expr[0] == "FUNC":
            pass
        elif split_expr[0] == "MAG":
            if split_expr[1] == "int":
                pass
            elif split_expr[1] == "float":
                if split_expr[2] == "." or split_expr[2] in self.numbers:
            else:
                raise Exception("pymagc translator : Type cannot be turned into Magnum")
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
        else:
            raise Exception("pymagc translator : Unsupported / unknown type for make")
        self.stored_vars.update({args[1]: args[0]})
        line_to_write += expr + ';'
        self.write(line_to_write)

    def set(self, args):  # args = [name, expr]
        if not (args[0] in self.stored_vars):
            raise Exception("pymagc translator : Unknown variable")
        if self.stored_vars[args[0]] == "int":
            expr = self.translate_expr_int(args[1])
        elif self.stored_vars[args[0]] == "float":
            expr = self.translate_expr_float(args[1])
        else:
            raise Exception("pymagc translator : Unsupported / unknown type for set")
        line_to_write = f"{args[0]} = " + expr + ";"
        self.write(line_to_write)

    def translate_line(self, line):
        print("line", line)
        words = self.split(line)
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
        for line in lines:
            self.translate_line(line)
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


TO DO: set ";" as seperator

CYCLE i 0 n {
    SET ...
    ...
}

FUNC a ((type name) (type name) (type name) ... ) (output type) {
}

'''
