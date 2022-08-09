import subprocess
import multiprocessing
import random
import fortune
import argparse
import json


parser = argparse.ArgumentParser(description='Run tests on thread assigments.')
parser.add_argument('--no-pi', action='store_false', dest="pi",
                    help='don\'t run tests on the pi executable')
parser.add_argument('--no-exchange', action='store_false', dest="exchange",
                    help='don\'t run tests on the exchange executable')
parser.add_argument('--no-generate', action='store_false', dest="generate",
                    help='don\'t generate new input file')
parser.add_argument('--grade', action='store_true', dest="grade",
                    help='generate the output for gradescope')
parser.add_argument('--file', dest="input_file",
                    help='use this input file for the exchange instead')


grade = {
    "output": "",
    "tests": [
        {
            "score": 0.0,
            "max_score": 3.0,
            "name": "Parallel pie",
            "output": "",
            "visibility": "visible"
        },
        {
            "score": 0.0,
            "max_score": 6.0,
            "name": "Exchanging messages",
            "output": "",
            "visibility": "visible"
        },
    ]
}

args = parser.parse_args()


n_cores = multiprocessing.cpu_count()
DEBUG = 0

result = {}

def make(args):
    argv = ["make"]
    if args is None:
        args = []
    if not isinstance(args, list):
        args = [args]
    argv.extend(args)
    if DEBUG: print(f"running: {argv}")
    proc = subprocess.run(argv, capture_output=True, shell=False, timeout=30)
    stdout = proc.stdout.decode("utf-8")
    stderr = proc.stderr.decode("utf-8")
    if proc.returncode != 0:
        print("Error making:")
        print(f"Output:\n{stdout}")
        print(f"Error:\n{stderr}")
    if DEBUG:
        print(f"Debug output:\n{stdout}")



def run(exec, args=None):
    if args is None:
        args = []
    argv = [exec]
    if not isinstance(args, list):
        args = [args]
    argv.extend(args)
    proc = subprocess.run(argv, capture_output=True, shell=False, timeout=120)
    stdout = proc.stdout.decode("utf-8")
    stderr = proc.stderr.decode("utf-8")
    if proc.returncode != 0:
        print("Error running:")
        print(f"{stderr}")
    if DEBUG:
        print(f"Debug output:\n{stdout}")

    return stdout, stderr


if(args.pi):
    runtimes = []
    pass_pi_correctness = True
    pass_pi_timeliness = True
    for i in [1,2,4,8]:
        s = "s" if i>1 else ""
        print(f"Calculating PI with {i} thread{s}...")
        make(["clean"])
        make([f"CFLAGS=-DNUM_THREADS={i} -DNUM_POINTS=1000000000 -DDEBUG=0", "pi"])
        stdout, stderr = run("/usr/bin/time", ["-f", "%e", "./pi"])
        PI_estimate = float(stdout.split(" ")[-1])
        runtime = float(stderr.split("\n")[0])
        runtimes.append(runtime)
        print(f"Executed in {runtime} and calculated PI as {PI_estimate}\n")
        if(PI_estimate<3.14 or PI_estimate > 3.15):
            print(f"Something doesn't look good with that estimate 🤔\n")
            pass_pi_correctness = False
        if i>1:
            if i <= n_cores:
                if(runtimes[-1]/runtimes[-2] >= 1):
                    print(f"More threads not faster? 🤔\n")
                    pass_pi_timeliness = False
                elif(runtimes[-1]/runtimes[-2] >= 0.7):
                    print(f"Faster, but not much? 🤔\n")
                    pass_pi_timeliness = False
            elif (runtimes[-1]/runtimes[-2] >= 0.9):
                print(f"More threads not much faster? 🤔")
                print(f"Maybe I ran out of CPUs?")
                print(f"I'm using {i} threads on {n_cores} CPU cores!")
    if not pass_pi_correctness:
        grade["tests"][0]["score"] = 0.0
    elif not pass_pi_timeliness:
        grade["tests"][0]["score"] = 1.5
    else:
        grade["tests"][0]["score"] = 3.0

            
def generate_messages(file, n_clients, n_messages):
    with open(file, "w") as f:

        for i in range(n_messages):
            source = random.randint(1, n_clients)
            destination = random.randint(1, n_clients)
            message = fortune.get_random_fortune("./linux").replace("\n", " ").replace(":", "-")
            if (len(message)>998):
                message = message[0:998]
            f.write(f"{source} {destination} {message}\n")

def compare_results(expected, result):
    for client_idx in range( len(expected) ):
        if len(expected[client_idx]) != len(expected[client_idx]):
            return False
        for string_idx in range(len(expected[client_idx])):
            if expected[client_idx][string_idx] != result[client_idx][string_idx]:
                # print(f"Backtrace:\n{expected[client_idx][string_idx-1]}\n{expected[client_idx][string_idx-0]}\n{expected[client_idx][string_idx+1]}\n{expected[client_idx][string_idx+2]}")
                # print(f"Expected:\n{expected[client_idx][string_idx]}\n\n Got:\n{result[client_idx][string_idx]}\n")
                return False 

    return True 

if (args.exchange):
    print(f"Running exchange...")
    i=0
    n_exchanges = 4
    clients = 4
    ifile = "file.txt"
    # ifile = "tests/test0.txt"
    results = []
    expected_results = []
    for i in range(clients):
        results.append([])
        expected_results.append([])

    print(f"Making exchange...")
    make(["clean"])
    make([f"CFLAGS=-DN_EXCHANGES={n_exchanges} -DN_CLIENTS={clients} -DDEBUG=0", "exchange"])

    if(args.generate and args.input_file is None):
        print(f"Generating messages...")
        generate_messages(ifile, clients, 1000)
    if(args.input_file is not None):
        ifile = args.input_file

    print(f"Running program...")
    stdout, stderr = run("./exchange", [ifile])

    print(f"Generating solution...")
    with open(ifile, "r") as f:
        line = f.readline()
        while len(line) != 0:
            destination = int(line.split(" ")[1])-1
            message = " ".join(line.split(" ")[2:]).strip()
            expected_results[destination].append(message)
            line = f.readline()

    for i in range(clients):
        expected_results[i].sort()

    print(f"Collection output...")
    for output in stdout.split("\n"):
        if(len(output)!=0):
            lst = output.split(":")
            receiver = int(lst[0])-1
            message = lst[1].strip()
            results[receiver].append(message)
    for i in range(clients):
        results[i].sort()

    if (compare_results(expected_results, results) ):
        print("Looks good :)")
        grade["tests"][1]["score"] = 6.0
        if("ERROR: Tried to check if" in stderr):
            print("But you are looping those is_free/is_full functions a lot! Geez, the CPU is so busy ;)")
            grade["tests"][1]["score"] = 4.0

    else:
        print("Something is not right! Some messages seem to be missing or repeated?")
        grade["tests"][1]["score"] = 0.0



    with open("stdout.txt", "w") as f:
        f.write(stdout)
    with open("stderr.txt", "w") as f:
        f.write(stderr)
    with open("expected.txt", "w") as f:
        for client_idx in range( len(expected_results) ):
            f.write(f"Client {client_idx} received:\n")
            for string_idx in range(len(expected_results[client_idx])):
                f.write(f"  - {expected_results[client_idx][string_idx]}\n")

if args.grade:
    with open("grade.json", "w") as f:
        f.write(json.dumps(grade, indent=4))