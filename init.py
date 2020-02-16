#!/usr/bin/env python3
import os
import shutil

if __name__ == '__main__':

    if not os.path.exists("conanws.yml"):
        shutil.copy("conanws.base.yml", "conanws.yml")
