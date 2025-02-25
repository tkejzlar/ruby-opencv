#!/usr/bin/env ruby

CC = RbConfig::CONFIG['CC']
if CC =~ /clang/
  RbConfig::MAKEFILE_CONFIG['try_header'] = :try_cpp
  RbConfig::CONFIG['CPP'] = "#{CC} -E"
elsif RbConfig::CONFIG['arch'] =~ /mswin32/
  RbConfig::MAKEFILE_CONFIG['try_header'] = :try_cpp
  RbConfig::CONFIG['CPP'] = "#{CC} /P"
end

require "mkmf"

def cv_version_suffix(incdir)
  major, minor, subminor = nil, nil, nil
  open("#{incdir}/opencv2/core/version.hpp", 'r') { |f|
    f.read.lines.each { |line|
      major = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_EPOCH|CV_MAJOR_VERSION)\s+(\d+)\s*\Z/
      minor = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_MAJOR|CV_MINOR_VERSION)\s+(\d+)\s*\Z/
      subminor = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_MINOR|CV_SUBMINOR_VERSION)\s+(\d+)\s*\Z/
    }
  }
  major + minor + subminor
end

# Quick fix for 2.0.0
# @libdir_basename is set to nil and dir_config() sets invalid libdir '${opencv-dir}/' when --with-opencv-dir option passed.
@libdir_basename ||= 'lib'
incdir, libdir = dir_config("opencv", "/opt/homebrew/Cellar/opencv/4.11.0_1/include", "/opt/homebrew/Cellar/opencv/4.11.0_1/lib")
dir_config("libxml2", "/usr/include", "/usr/lib")

opencv_headers = ["/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/core/core_c.h", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/core/core.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/imgproc/imgproc_c.h",
                  "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/imgproc/imgproc.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/video/tracking.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/features2d/features2d.hpp",
                  "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/flann/flann.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/calib3d/calib3d.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/objdetect/objdetect.hpp",
                  "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/legacy/compat.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/legacy/legacy.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/highgui/highgui_c.h",
                  "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/highgui/highgui.hpp", "/opt/homebrew/Cellar/opencv/4.11.0_1/include/opencv4/opencv2/photo/photo.hpp"]
opencv_headers_opt = ["opencv4/opencv2/nonfree/nonfree.hpp"]

opencv_libraries = ["opencv_calib3d", "opencv_core", "opencv_features2d",
                    "opencv_flann", "opencv_highgui", "opencv_imgproc",
                    "opencv_ml", "opencv_objdetect", "opencv_video", "opencv_photo"]
opencv_libraries_opt = ["opencv_gpu", "opencv_nonfree"]

puts ">> Check the required libraries..."
if $mswin or $mingw
  suffix = cv_version_suffix(incdir)
  opencv_libraries.map! { |lib| lib + suffix }
  opencv_libraries_opt.map! { |lib| lib + suffix }
  have_library("msvcrt")
  if $mswin
    $CFLAGS << ' /EHsc'
    CONFIG['CXXFLAGS'] << ' /EHsc'
  end
else
  have_library("stdc++")
end


opencv_libraries.each { |lib| raise "#{lib} not found." unless have_library(lib) }
opencv_libraries_opt.each { |lib| warn "#{lib} not found." unless have_library(lib) }

# Check the required headers
puts ">> Check the required headers..."
opencv_headers.each { |header| raise "#{header} not found." unless have_header(header) }
opencv_headers_opt.each { |header| warn "#{header} not found." unless have_header(header) }
have_header("stdarg.h")

if $warnflags
  $warnflags.slice!('-Wdeclaration-after-statement')
  $warnflags.slice!('-Wimplicit-function-declaration')
end

# Quick fix for 1.8.7
$CFLAGS << " -I#{File.dirname(__FILE__)}/ext/opencv"

# Create Makefile
create_makefile('opencv')

