# breaklee

Copyright (c) 2021 Notestsoft (notestsoft@proton.me).

[LICENSE](LICENSE).

![logo](https://raw.githubusercontent.com/notestsoft/breaklee/main/logo.png) 

--------------

* [Introduction](#introduction)
* [Requirements](#requirements)
* [Build](#build)
* [Database Setup](#database-setup)
* [Configuration](#configuration)
* [Accounts](#accounts)
* [Copyright](#copyright)

## Introduction

**Disclaimer:**

This software is not affiliated with any specific game title or its developers. It is designed solely for educational and experimental purposes. The use of this software for any illegal activities, including but not limited to copyright infringement, unauthorized access, or distribution of proprietary materials, is strictly prohibited.

We do not condone or support any unlawful actions facilitated by the use of this software. Users are solely responsible for ensuring their compliance with all relevant laws and regulations governing their use of this software.

By accessing and using this software, you agree to abide by these terms and acknowledge that any misuse of this software is entirely at your own risk.

## Requirements

| Requirement      | Tool           | Version                     |
|------------------|----------------|-----------------------------|
| Package Manager  | Conan          | [Conan](https://conan.io/)  |
| Build Tools      | CMake          | [CMake](https://cmake.org/) |

## Build

| Supported Platforms |
|---------------------|
| Windows             |
| Linux               |

- Install dependencies `conan install . --output-folder=Build --build=missing -s build_type=Debug|Release`
- Configure preset `cmake --preset conan-default`
- Use the Build/conan_toolchain.cmake file as toolchain in cmake.
- Use CMake along with your preferred build tools to create the project.

## Database Setup

1. **Create databases**:
   - Create databases by using the 0000_CreateDatabase.sql files inside the /Database folder.
   - Any database supporting odbc will be fine
   - Migrate the language support of the migration files for your desired database
   - Configure the driver inside LoginSvr.ini and MasterDBAgent.ini

## Configuration

1. **Config files**: Copy all `*.ini` files from the `Config` folder of the project source to the application folder where the build executables are located.
   
2. **Configuration Settings**:
   - Update the `RuntimeDataPath` setting in your `WorldSvr.ini` file to point to the `Data` folder of your client.
   - Update the `ServerDataPath` setting in your `WorldSvr.ini` file to point to the `ServerData` folder located in the project source.
   - Update the `ScriptDataPath` setting in your `WorldSvr.ini` file to point to the `Scripts` folder located in the project source.

3. **Connection Configuration**:
   - Configure your database connection settings in the `[AuthDB]` section of `LoginSvr.ini` config file.
   - Configure your database connection settings in the `[MasterDB]` section of `MasterSvr.ini` config file.

## Accounts

- New accounts will be automatically inserted into the database when attempting to log in with a non-existing user id in the client.

## Reporting issues

Issues can be reported via the [Github issue tracker](https://github.com/notestsoft/breaklee/labels/Branch-main).

Please take the time to review existing issues before submitting your own to prevent duplicates.

## Copyright

Copyright (c) 2021 Notestsoft (notestsoft@proton.me).

License: MIT

[LICENSE](LICENSE).
