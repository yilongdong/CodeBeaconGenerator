import os, re


def find_all_file(base):
    for root, ds, fs in os.walk(base):
        for f in fs:
            if re.match(r'.*\.cpp', f):
                fullname = os.path.join(root, f)
                yield fullname


def main():
    base = '/Users/dongyilong/clang-llvm/llvm-project/clang-tools-extra/code-beacon-gen/'
    for source_file in find_all_file(base):
        # print(source_file)
        bin = '/Users/dongyilong/clang-llvm/llvm-project/release-build/clang-tools-extra/bin/code-beacon-gen'
        db_dir = '/Users/dongyilong/clang-llvm/llvm-project/release-build/clang-tools-extra'
        cmd = '{} {} -p {}'.format(bin, source_file, db_dir)
        print("cmd = {}".format(cmd))
        print(os.popen(cmd).read())


if __name__ == '__main__':
    main()
