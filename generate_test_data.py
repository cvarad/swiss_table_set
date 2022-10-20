from random import randrange

def generate_random(filename, num_range):
    sizes = map(int, [1e3, 5e4, 1e6])
    for size in sizes:
        with open(f'tests/{filename}_{size}.txt', 'w') as f:
            for _ in range(size):
                f.write(str(randrange(*num_range)) + '\n')
    
def main():
    generate_random('short_ints', (-2**15, 2**15))
    generate_random('ints', (-2**31, 2**31))
    generate_random('long_ints', (-2**63, 2**63))

if __name__ == '__main__':
    main()