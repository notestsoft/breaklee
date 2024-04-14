# breaklee

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

breaklee is based mostly in C.

It is completely open source; community involvement is highly encouraged.
If you wish to contribute ideas or code, please read the [Contribution Guidelines](CONTRIBUTING.md).

**Disclaimer:**

This software is not affiliated with any specific game title or its developers. It is designed solely for educational and experimental purposes. The use of this software for any illegal activities, including but not limited to copyright infringement, unauthorized access, or distribution of proprietary materials, is strictly prohibited.

We do not condone or support any unlawful actions facilitated by the use of this software. Users are solely responsible for ensuring their compliance with all relevant laws and regulations governing their use of this software.

By accessing and using this software, you agree to abide by these terms and acknowledge that any misuse of this software is entirely at your own risk.

## Requirements

| Requirement      | Tool           | Version            |
|------------------|----------------|--------------------|
| Build Tools      | CMake          | [CMake](https://cmake.org/) Version 3.21.0 |
| Database         | MariaDB Server | [MariaDB Server](https://mariadb.org/) Version tested 11.3.2 |
| Client           |                | Region NA v596     |

## Build

| Supported Platforms |
|---------------------|
| Windows x64         |

1. **Project Setup**: Use CMake along with your preferred build tools to create the project.

## Database Setup

1. **Maria DB Server Instances**:
   - Create two separate instances of Maria DB Server on your machine, each with its own distinct port.

2. **Database Initialization**:
   - Run all `AuthDB/*.sql` files in the correct order within the AuthDB database instance.
   - Run all `MasterDB/*.sql` files in the correct order within the MasterDB database instance.

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

License: MIT

Read [LICENSE](LICENSE).
