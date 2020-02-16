#!/usr/bin/env python3

import subprocess

if __name__ == '__main__':
    branch_name = subprocess.check_output('git rev-parse --abbrev-ref HEAD'.split(' ')).decode('ascii').strip()

    image_name = 'lmlib_ci_{}'.format(branch_name).lower()

    # Build image.
    subprocess.check_call('docker build . -t {}'.format(image_name).split(' '))

    # Build and run tests in new container.
    command = 'cmake .. && make -j test_lmlib && ./test_lmlib'

    result = 1

    try:
        result = subprocess.call([
            'docker',
            'run', '--name',
            'lmlib_ci',
            image_name,
            '/bin/bash',
            '-c',
            command,
        ])

        passed = result == 0

        print('CI passed.' if passed else 'CI failed.')
    finally:
        subprocess.check_call([
            'docker',
            'rm',
            'lmlib_ci',
        ])

    exit(result)
