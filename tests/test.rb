require 'rbconfig'
require 'fileutils'

STDOUT.sync = true

def main

  Rollcut.init_tmp

  [0, 1, 2**8, 2**16, 2**24, rand(2**16).to_i, rand(2**24).to_i].each{|length|
    [:bin, :txt].each{|mode|
      f1= Rand.new_file(length, mode)
      Rollcut.cut_file(f1)
      f2= Rollcut.uncut_file(f1)
      Log.info "Compare f1,f2: #{FileUtils.identical?(f1,f2)}\n"      
    }
  }
end

module Rollcut

  case RbConfig::CONFIG['host_os']
  when /mac|darwin/
    BIN = "../bin/osx/rollcut"
    TMP = "/var/tmp/rollcut/"
    UNCUT = "; cat *.rollcut > unrollcut.file"
  when /linux|cygwin/
    BIN = "../bin/linux/rollcut"
    TMP = "/var/tmp/rollcut/"
    UNCUT = "; cat *.rollcut > unrollcut.file"
  when /bsd/
    BIN = "../bin/bsd/rollcut"
    TMP = "/var/tmp/rollcut/"
    UNCUT = "; cat *.rollcut > unrollcut.file"
  when /mswin|win|mingw/
    BIN = "..\\bin\\win32\\rollcut.exe"
    TMP = "c:\\temp\\rollcut\\"
    UNCUT = "& copy /b *.rollcut unrollcut.file"
  end
  
  def self.init_tmp
    Log.info "Cleaning rollcut temp folder...\n"
    FileUtils.rm_rf Rollcut::TMP
    FileUtils.mkdir_p Rollcut::TMP
    Log.ok
  end
  
  def self.cut_file(file_name)
    Log.info "Cutting file '#{file_name}'..."
    command="#{BIN} #{file_name}"
    `#{command}`
    Log.ok
  end
  
  def self.uncut_file(file_name)
    Log.info "Uncutting file '#{file_name}'..."
    command= "cd #{file_name}.rollcuts #{Rollcut::UNCUT}"
    Log.info command
    `#{command}`
    Log.ok
    return File.join("#{file_name}.rollcuts","unrollcut.file")
  end
  
end

module Rand
  @modes={:txt=>"w", :bin=>"wb"}
  @nfiles={:txt=>0, :bin=>0}
  
  def self.new_file(length, mode)
    name = self.new_name(mode)
    Log.info "Creating new random file #{"%.3f"%(length/1024.0)}Kb '#{name}'..."
    File.open(name, @modes[mode]){|f| f.write string(length, mode)}
    Log.ok
    return name
  end
  
  def self.new_name(mode)
    new_name = Rollcut::TMP+"rollcut_test.#{"%03d" % @nfiles[mode]}.#{mode}"
    @nfiles[mode] +=1
    return new_name
  end
  
  def self.string(length, mode)
    case mode
    when :txt
      self.txt_string(length)
    when :bin
      self.bin_string(length)
    end
  end
  
  def self.txt_string(length)
    str=""
    (1..length).each{str<<(48+rand(74)).chr}
    return str
  end
  
  def self.bin_string(length)
    str=""
    (1..length).each{str<<rand(256).chr}
    return str
  end
  
end

module Log
  @@last_time=Time.now
  
  def self.info(message)
    @@last_time=Time.now
    print "\n#{@@last_time} - #{message}"
  end
  
  def self.ok
    print "   [OK #{"%02.2f"%(Time.now-@@last_time)}sec]"
  end

end

main
