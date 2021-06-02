#!/usr/bin/env python
#coding: utf-8

"""
Generate ffmpeg-config.cmake
"""

# note: the order matters
# ref: https://blog.csdn.net/zijin0802034/article/details/52779791
ffmpeg_lib_desc = {
    'libavformat': '58.71.100',
    'libavdevice': '58.12.100',
    'libavcodec': '58.129.100',
    'libavutil': '56.67.100',
    'libswscale': '5.8.100',
    'libswresample': '3.8.100',
    'libavfilter': '7.109.100',
    'libavresample': '4.0.0',
}

content_lines = [
    'set(ffmpeg_path "${CMAKE_CURRENT_LIST_DIR}")',
    '',
    'set(FFMPEG_EXEC_DIR "${ffmpeg_path}/bin")',
    'set(FFMPEG_LIBDIR "${ffmpeg_path}/lib")',
    'set(FFMPEG_INCLUDE_DIRS "${ffmpeg_path}/include")',
    '',
]


# setup library names
content_lines.append('# library names')
content_lines.append(
    'set(FFMPEG_LIBRARIES',
)

for key, value in ffmpeg_lib_desc.items():
    lib_name = key
    lib_version = value
    content_lines.append(
        '    ${FFMPEG_LIBDIR}/' + lib_name + '.a'
    )
content_lines.append('    pthread')
content_lines.append(')')
content_lines.append('')


# setup library found
content_lines.append('# found status')
for key, value in ffmpeg_lib_desc.items():
    lib_name = key
    content_lines.append(
        'set(FFMPEG_{:s}_FOUND TRUE)'.format(lib_name)
    )
content_lines.append('')

# setup library versions
content_lines.append('# library versions')
for key, value in ffmpeg_lib_desc.items():
    lib_name = key
    lib_version = value
    content_lines.append(
        'set(FFMPEG_{:s}_VERSION {:s})'.format(lib_name, lib_version)
    )
content_lines.append('')

# final stuffs
content_lines.append('set(FFMPEG_FOUND TRUE)')
content_lines.append('set(FFMPEG_LIBS ${FFMPEG_LIBRARIES})')

# summary up and write
fout = open('ffmpeg-config.cmake', 'w')
for line in content_lines:
    fout.write(line + '\n')
fout.close()


