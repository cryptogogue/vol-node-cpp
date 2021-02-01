platform :osx, '10.15'
inhibit_all_warnings!

def shared_pods
	pod 'OpenSSL-Universal', '~> 1.0'
end

target 'server' do
	shared_pods
end

target 'simulator' do
	shared_pods
end

target 'util' do
	shared_pods
end

target 'volition-gtest' do
	shared_pods
end
