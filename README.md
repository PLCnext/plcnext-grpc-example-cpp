# PLCnext gRPC with C++ Example

This project shows how to use the PLCnext gRPC Service in a C++ application.
The PLCnext gRPC service description files are compiled and then used in a standalone C++ example application.
For more information about gRPC in PLCnext Technology see the [gRPC communication](https://www.plcnext.help/te/Communication_interfaces/gRPC_Introduction.htm) topic in the [PLCnext Info Center](https://www.plcnext.help).

This example was tested with the following PLCnext controllers:

* AXC F 2152 FW 2022.6
* AXC F 3152 FW 2023.0

An Example for the toolchain configuration is included as Visual Studio Code [cmake-kits.json](.vscode/cmake-kits.json) file.

## Build the example

How to compile the `*.proto` service files is described in the [PLCnext gRPC repository](https://github.com/PLCnext/gRPC). This compile step is done by the CMake module [`FindPlcnextGrpc.cmake`](cmake/FindPlcnextGrpc.cmake) that is included in this example.

The Protobuf compiler `protoc` is included in the PLCnext SDK. Just declare the CMake variable `CMAKE_PROGRAM_PATH` with the location of the `protoc` executable in the native PLCnext SDK. See the [`cmake-kits.json`](.vscode/cmake-kits.json) file for a working example.

However the gRPC C++ Protocol Buffers plugin `grpc_cpp_plugin` is only available in the SDK sysroot of the target controller. So the plugin can not be used on the development host. Therefor the gRPC C++ plugin has to be compiled from source. This example includes tasks for Visual Studio Code to fetch the gRPC sources an compile them on the local machine. The following tasks are used to fetch and compile gRPC from source:

* Fetch external content: gRPC
* gRPC plugins: CMake configure
* gRPC plugins: CMake build

The vscode task `Fetch external content: gRPC` clones the gRPC git repository into the folder `build/external/grpc`. It can then be configured and build with the other two provided vscode tasks that call CMake. The CMake build directory is defined as `build/cmake/external/grpc` in the vscode task `gRPC plugins: CMake configure`.

Pre-requisites to build gRPC:

```bash
apt-get install build-essential autoconf libtool pkg-config cmake
```

Be aware that the version of gRPC has to match the version that is included in the used PLCnext target. The Visual Studio Code task `Fetch external content: gRPC` has to be adjusted to clone the right git tag. You can find the used version information in the file `/usr/lib/cmake/grpc/gRPCConfigVersion.cmake` that is included in the target sysroot of the SDK. For example for the SDK of the controller AXC F 2152 this is the file `sysroots/cortexa9t2hf-neon-pxc-linux-gnueabi/usr/lib/cmake/grpc/gRPCConfigVersion.cmake`.

For CMake to be able to find the compiled gRPC C++ Protobuf plugin `grpc_cpp_plugin` the CMake module [`FindPlcnextGrpc.cmake`](cmake/FindPlcnextGrpc.cmake) has to be given the path as hint where to search for the plugin binaries. This is done by declaring the CMake variable `CMAKE_PROGRAM_PATH` with the location of the CMake build directory for gRPC. See the [`cmake-kits.json`](.vscode/cmake-kits.json) file for a working example of how to configure CMake.

## Run the example

To be able to run the example on the target controller the `libabsl*.so` shared object libraries have to be copied from the SDK to the target. This was tested with the controller AXC F 2152 firmware 2022.6. This step is not necessary with the controller AXC F 3152 firmware 2023.0.

To be able to copy the files over ssh you need root access to the target. Run the copy command from your development machine and copy the library files from your PLCnext SDK target sysroot to the target:

```bash
scp sysroots/cortexa9t2hf-neon-pxc-linux-gnueabi/usr/lib/libabsl*.so root@192.168.1.10:/usr/lib
```

Then login into your target as root an run:

```bash
ldconfig
```

to create the necessary links and update the linker cache.

Copy the example executable to the controller and call it with a valid port name of the type uint16 to read and write to:

```bash
./plcnext-grpc-example Arp.Plc.Eclr/wDO16
```

## Useful hints

A good lookup on how to use the generated C++ protocol buffer classes is the [C++ Generated Code Guide](https://protobuf.dev/reference/cpp/cpp-generated/) from [Protocol Buffers Documentation](https://protobuf.dev/).
