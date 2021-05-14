project('kiwix-lib', 'cpp',
  version : '10.0.0', # Also change this in android-kiwix-lib-publisher/kiwixLibAndroid/build.gradle
  license : 'GPLv3+',
  default_options : ['c_std=c11', 'cpp_std=c++11', 'werror=true'])

compiler = meson.get_compiler('cpp')

wrapper = get_option('wrapper')

static_deps = wrapper.contains('android') or wrapper.contains('java') or get_option('default_library') == 'static'
if wrapper.contains('android')
  extra_libs = ['-llog']
else
  extra_libs = []
endif

if wrapper.contains('java')
  add_languages('java')
endif

# See https://github.com/kiwix/kiwix-lib/issues/371
if ['arm', 'mips', 'm68k', 'ppc', 'sh4'].contains(target_machine.cpu_family())
  extra_libs += '-latomic'
endif

if (compiler.get_id() == 'gcc' and build_machine.system() == 'linux') or target_machine.system() == 'freebsd'
  # C++ std::thread is implemented using pthread on linux by gcc
  thread_dep = dependency('threads')
else
  thread_dep = dependency('', required:false)
endif
libicu_dep = dependency('icu-i18n', static:static_deps)
pugixml_dep = dependency('pugixml', static:static_deps)
libcurl_dep = dependency('libcurl', static:static_deps)
microhttpd_dep = dependency('libmicrohttpd', static:static_deps)
zlib_dep = dependency('zlib', static:static_deps)
xapian_dep = dependency('xapian-core', static:static_deps)

if compiler.has_header('mustache.hpp')
  extra_include = []
elif compiler.has_header('mustache.hpp', args: '-I/usr/include/kainjow')
  extra_include = ['/usr/include/kainjow']
else
  error('Cannot found header mustache.hpp')
endif

libzim_dep = dependency('libzim', version : '>=7.0.0', static:static_deps)
if not compiler.has_header_symbol('zim/zim.h', 'LIBZIM_WITH_XAPIAN')
  error('Libzim seems to be compiled without xapian. Xapian support is mandatory.')
endif


extra_cflags = ''
if target_machine.system() == 'windows' and static_deps
  add_project_arguments('-DCURL_STATICLIB', language : 'cpp')
  extra_cflags += '-DCURL_STATICLIB'
endif

all_deps = [thread_dep, libicu_dep, libzim_dep, pugixml_dep, libcurl_dep, microhttpd_dep, zlib_dep, xapian_dep]

inc = include_directories('include', extra_include)

conf = configuration_data()
conf.set('VERSION', '"@0@"'.format(meson.project_version()))

if build_machine.system() == 'windows'
     extra_link_args = ['-lshlwapi', '-lwinmm']
else
     extra_link_args = []
endif

subdir('include')
subdir('scripts')
subdir('static')
subdir('src')
subdir('test')

pkg_requires = ['libzim', 'icu-i18n', 'pugixml', 'libcurl', 'libmicrohttpd', 'xapian-core']

pkgconfig = import('pkgconfig')
pkgconfig.generate(
  kiwixlib,
  name: 'kiwix',
  description: 'A library that contains a lot of things used by used by other kiwix programs',
  url: 'https://github.com/kiwix/kiwix-lib',
  requires: pkg_requires + extra_libs,
  extra_cflags: extra_cflags
)
