# coding: utf-8

from dataclasses import dataclass
import time

@dataclass
class AutoTimer:
    def __init__(self, name):
        self._start_time = None
        self._name = name

    def start(self):
        """Start a new timer"""
        if self._start_time is not None:
            raise TimerError(f"Timer is running. Use .stop() to stop it")
        self._start_time = time.perf_counter()

    def stop(self):
        """Stop the timer, and report the elapsed time"""
        if self._start_time is None:
            raise TimerError(f"Timer is not running. Use .start() to start it")

        elapsed_time = time.perf_counter() - self._start_time
        self._start_time = None
        print(f"{self._name:s} took time: {elapsed_time:0.4f} seconds")

    def __enter__(self):
        """Start a new timer as a context manager"""
        self.start()
        return self

    def __exit__(self, *exc_info):
        """Stop the context manager timer"""
        self.stop()


def test():
    """
    download and print the latest tutorial from Real Python
    """

    from reader import feed

    # tic = time.perf_counter()
    # tutorial = feed.get_article(0)
    # toc = time.perf_counter()
    # print(f"!!!Downloaded the tutorial in {toc - tic:0.4f} seconds")

    with AutoTimer('download'):
        tutorial = feed.get_article(0)

    #print(tutorial)

if __name__ == '__main__':
    test()