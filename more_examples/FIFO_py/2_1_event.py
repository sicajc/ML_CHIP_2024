def f():
    yield 2
    print("Done 2!")
    yield 0
    print("Done 0!")
    yield 1
    print("Done 1!")

event_pending_on_list = [list() for _ in range(100)]

def WaitOnNextEvent(t):
    try:
        waiting_on = next(t)
        event_pending_on_list[waiting_on].append(t)
    except StopIteration:
        pass

def main_loop(threads):
    TRIGGER = [0, 1, 2, 3, 0, 1, 2]
    for t in threads:
        WaitOnNextEvent(t)
    for trigger in TRIGGER:
        print("handling {}".format(trigger))
        handling, event_pending_on_list[trigger] = event_pending_on_list[trigger], list()
        for t in handling:
            WaitOnNextEvent(t)

main_loop([f()])
