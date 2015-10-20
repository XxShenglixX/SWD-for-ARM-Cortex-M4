PROJECT_CEEDLING_ROOT = "vendor/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/ceedling.rb"

Ceedling.load_project

task :default => %w[ test:all release ]

load "scripts/hw.rb"
load "scripts/probe.rb"
