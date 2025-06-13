import sqlite3
import subprocess
from tqdm import tqdm

class TestSuite:
    def __init__(self):
        pass

    def runTests(self, runner_callback):
        pass

class EndgamePuzzles(TestSuite):
    def __init__(self):
        super().__init__()
        self.db = sqlite3.connect('puzzles.sqlite3')
        self.testCases = {}
        self.LoadData()

    def LoadData(self):
        result = self.db.execute("SELECT * FROM data")

        for FEN, mateIn in result.fetchall():
            if mateIn not in self.testCases:
                self.testCases[mateIn] = []
            self.testCases[mateIn].append(FEN)

    def runTests(self, runner_callback):
        self.runMateInTests(2, runner_callback)

    def runMateInTests(self, mateIn, runner_callback):
        success_count, total = 0, 0

        bar = tqdm(self.testCases[mateIn])

        for FEN in bar:
            success = False
            try:
                success = int(runner_callback([["--fen", FEN], ["--mateIn", ""]]).strip()) == mateIn
            except:
                print(f"Error running {FEN}")
                exit(0)
            if success:
                success_count += 1
            else:
                print("\n", "Test failure:", FEN)
            total += 1
            bar.set_description(f"passing {success_count}/{total}, {success_count/total * 100:.2f}%")

class TestRunner:
    def __init__(self, exe):
        self.exe = exe

    def runner_callback(self, args):
        to_run = [self.exe]
        for arg, arg_value in args:
            to_run.append(arg)
            to_run.append(arg_value)

        result = subprocess.run(to_run, capture_output = True)
        return result.stdout.decode()

    def runTest(self, testSuite: TestSuite):
        testSuite.runTests(self.runner_callback)

if __name__ == '__main__':
    test = EndgamePuzzles()

    test_runner = TestRunner("../src/cmake-build-debug/Chess")

    test_runner.runTest(test)
