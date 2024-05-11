def f():
    for i in range(5):
        print("Function f, {}".format(i))
        yield
    print("Function f finished")

def g():
    for i in range(3):
        print("Function g, {}".format(i))
        yield
    print("Function g finished")

def main_loop():
    from itertools import zip_longest
    for dummy in zip_longest(f(), g()):
        pass

main_loop()
