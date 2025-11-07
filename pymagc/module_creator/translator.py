class Translator:

    def __init__(self, file_path):
        self.file_path = file_path
        self.processes = {
            "MAKE": [self.make, 3],
            "SET": [self.set, 2]
        }
        self.numbers = [str(i) for i in range(10)]
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
            if char == ' ':
                if depth == 0:
                    split_string.append(text[last_end:idx])
                    last_end = idx+1
            elif char == '(':
                depth += 1
            elif char == ')':
                depth -= 1
        split_string.append(text[last_end:])
        return split_string

    def split(self, string):
        depth = 0
        last_end = 0
        split_string = []
        for idx, char in enumerate(string):
            if char == ' ':
                if depth == 0:
                    split_string.append(string[last_end:idx])
                    last_end = idx+1
            elif char == '(':
                depth += 1
            elif char == ')':
                depth -= 1
        split_string.append(string[last_end:])
        return split_string

    def write(self, line):
        self.translation.append(line)

    def translate_expr(self, expr, type):
        expr = expr[1:-1]
        split_expr = self.split(expr)
        print("expr", split_expr)
        expr_to_write = ''  # (42 * 43) + (a * b) -> [(42 * 43), +, (a * b)]
        return expr_to_write

    def translate_script(self):
        pass

    def make(self, args):  # args = [type, name, expr]
        if args[1] in self.stored_vars:
            raise Exception("pymagc translator : Variable already exists")
        else:
            if args[0] == 'int':
                line_to_write = f'int {args[1]} = '
            elif args[0] == 'mag':
                line_to_write = f'struct magnum * {args[1]} = '
            else:
                raise Exception("pymagc translator : Unsupported / unknown type")
            self.stored_vars.update({args[1]: args[0]})
            line_to_write += self.translate_expr(args[2], args[0])
            self.write(line_to_write)

    def set(self, args):
        pass

    def translate_line(self, line):
        words = self.split(line)
        print("words", words)
        if words[0] in self.processes:
            n_of_args_needed = self.processes[words[0]][1]
            if len(words) - 1 < n_of_args_needed:
                raise Exception("pymagc translator : Missing arguments")
            else:
                args = words[1:n_of_args_needed+1]
                self.processes[words[0]][0](args)

    def translate(self):
        with open(self.file_path, "r") as file:
            str_file = file.read()
            file.close()
        lines = self.split_lines(str_file.strip("\n"))
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
