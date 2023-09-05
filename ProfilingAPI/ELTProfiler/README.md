# XPlat CoreCLR Profiler with ELT

This sample shows a minimal CoreCLR profiler that setups the Enter/Leave hooks using `SetEnterLeaveFunctionHooks3WithInfo`

## Prerequisites

* CoreCLR Repository (build from source) Dependencies
* Clang 3.5  (Linux)
* Visual Studio 2022 (Windows)

## Building on Windows

### Environment

All instructions must be run on the ``VS 2022 x64 Native Tools Command Prompt``.

```batch
SET CORECLR_PATH=../coreclr # default
SET BuildOS=Windows # Windows(default)
SET BuildArch=x64 # x64 (default)
SET BuildType=Debug # Debug(default), Release
SET Output=ClrProfiler.dll # default
```

### Build
On the ``VS 2022 x64 Native Tools Command Prompt``.

* msbuild
* or open using Visual Studio 2017 Community and Build

### 테스트

ELTProfiler, ConsoleTest 프로젝트를 빌드 후 Bin 폴더에 있는 run.bat 파일을 실행하면 기본적인 테스트가 가능합니다.

```batch
@echo off

SET CORECLR_PROFILER={cf0d821e-299b-5307-a3d8-b283c03916dd}
SET CORECLR_ENABLE_PROFILING=1
SET CORECLR_PROFILER_PATH=%~dp0ClrProfiler.dll

SET PROFILE_BASETIME=1000
SET NAMESPACE_PREFIX=ProfilingTest

ConsoleTest.exe
```

## UDP monitoring

콘솔로 확인할 수 없거나 원격지에서 로그를 확인하고 싶을 때에는 udp-monitor 프로젝트를 빌드하고 실행합니다.
이후에 run.bat 파일을 실행하시면 모든 메시지가 아래의 이미지와 같이 UDP를 통해서 전달됩니다.

![](./pic-01.png)

## 사용된 라이브러리

### `Settings` Class

#### 용도:
`Settings` 클래스는 애플리케이션 설정 값을 로드하고 관리합니다. 환경 변수를 통해 외부에서 설정을 제공하게 됩니다.

#### 퍼블릭 멤버:

- `getInstance()`: 싱글톤 인스턴스를 반환합니다.
- `getProfileBaseTime()`: `PROFILE_BASETIME` 환경 변수의 값을 반환합니다.
- `getNamespacePrefix()`: `NAMESPACE_PREFIX` 환경 변수의 값을 반환합니다.

### `CallHistory` Class

#### 용도:
`CallHistory` 클래스는 함수 호출의 이력을 관리하며, 각 함수 호출의 시작과 종료, 그리고 해당 호출의 지속 시간을 기록합니다. 이 클래스를 사용하면 프로파일링 데이터를 기반으로 함수 호출 트리를 시각화할 수 있습니다.

#### 퍼블릭 멤버:

- `getInstance()`: 싱글톤 인스턴스를 반환합니다.
- `Enter(FunctionID functionId)`: 함수 호출의 시작을 기록합니다.
- `Leave(FunctionID functionId)`: 함수 호출의 종료를 기록하고 지속 시간을 계산합니다.
- `Tailcall(FunctionID functionId)`: 테일콜이 발생했음을 기록합니다.

### `PerformanceCounter` Class

#### 용도:
`PerformanceCounter` 클래스는 성능 측정을 위한 프로파일링 도구로, 특정 코드의 실행 시간을 측정합니다.

#### 퍼블릭 멤버:

- `getIncetance()`: 싱글톤 인스턴스를 반환합니다.
- `enter()`: 성능 측정을 시작하기 위한 타임스탬프를 저장합니다.
- `leave()`: 성능 측정을 종료하고 시작 이후의 시간(틱 단위)을 반환합니다.
- `leaveInMilliseconds()`: 성능 측정을 종료하고 시작 이후의 시간(밀리초 단위)을 반환합니다.

### `MethodList` Class

#### 용도:
`MethodList` 클래스는 .NET 함수의 식별자(`FunctionID`)를 해당 함수의 전체 이름으로 매핑하는 역할을 합니다.

#### 퍼블릭 멤버:

- `getIncetance()`: 싱글톤 인스턴스를 반환합니다.
- `GetName(FunctionID functionId)`: 주어진 `FunctionID`에 해당하는 함수의 이름을 반환합니다.

### `UdpSocket` Class

#### 용도:
`UdpSocket` 클래스는 UDP 소켓을 통해 텍스트 데이터를 전송하는 기능을 제공합니다. 프로파일링 데이터나 로그 메시지를 원격 시스템으로 전송하는 데 사용될 수 있습니다.

#### 퍼블릭 멤버:

- `getInstance()`: 싱글톤 인스턴스를 반환합니다.
- `sendText(const wstring& message)`: 주어진 메시지를 UDP 소켓을 통해 전송합니다.
