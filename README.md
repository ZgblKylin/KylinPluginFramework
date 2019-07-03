# KylinPluginFramework
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![LICENSE](https://img.shields.io/badge/license-MPL-green.svg)](https://www.mozilla.org/en-US/MPL/)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)

KPF(Kylin Plugin Framework) is a light weight plugin framework for Qt, free to cooperate with 3rd-party library without any injection.

# Features
- **Light weight**: Only a single dll file.
- **Dynamic load**: Load any QObject class to build your program architecture configuration file, just like building LEGO!
- **No injection**: Any QObject classes could be supported with a single line of declaration like `Q_DECL_METATYPE`, you can write it anywhere before loading the object. So you could configure any QObject classes from 3-rd party library without any injection.
- **EventBus**: Publish/Subscribe mechanism suppoted to decouple sender and receiver:
  - Better performance than signals&slots.
  - Easier to write than signals&slots.
  - Both synchronized and asynchronized events are supported, just like `Qt::DirectConnection` and `Qt::QueuedConnection`.
- **Better signal&slots**:
  - Connect signals and slots by configuration file is also supported, just like eventbus.
  - Published event could connect to signals/slots, signals could connect to subscribed event too.
- **Configure everything**:
  - Object's every property could be configured
  - QObject's object-tree-hierarchy is also supported.
  - Object can be initialized in sub-thread by configuration, you needn't write any code about it.
- **Component**: Seperate configeration files are supported -- a `component` file is used to describe a subset of the configuration, and it behaves like a single concrete object, to be configured in other components or main configuration file.
- **Powerful log system**: KPF use [log4qt](https://github.com/ZgblKylin/log4qt)(Not which one imported from log4j) to record logs.
- **Pentiful APIs**: Advanced user can control every behaviors described above.

# Docs
Only coments at present, sorry for this.

# Examples
There's a simply example in `test` folder, which shows the features with configuration file and several classes.

# Roadmap
1. Submit code before 2019.7.7
2. Finish documentation with Doxygen(See [Another repository](https://zgblkylin.github.io/Cpp-Utilities/) for example).
3. Migrate configuration file format from `json` to `xml`.
4. Migrate log system to [log4qt](https://github.com/ZgblKylin/log4qt)(The existing logging system in KPF is prototype of my log4qt repo).
5. Unit tests and CI support for travis, appveyor and coveralls.
6. Other further maintenance:
   1. Migrate log system to 
   2. Operational monitoring; 
   3. Hotswap support.
   4. ...
