cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

function(sign_mach_executable_binary file)
	set(ENTITLEMENTS_PLIST "${CMAKE_BINARY_DIR}/${CADABRA_ACE_BINARY}.entitlements.plist")
	file(
		WRITE ${ENTITLEMENTS_PLIST}
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
		"<plist version=\"1.0\">\n"
		"	<dict>\n"
		"		<key>get-task-allow</key>\n"
		"		<true/>\n\n"
		"		<key>com.apple.security.network.client</key>\n"
		"		<true/>\n\n"
		"		<key>com.apple.security.files.user-selected.read-write</key>\n"
		"		<true/>\n\n"
		"		<!--<key>com.apple.security.files.all</key>-->\n"
		"		<!--<true/>-->\n"
		"	</dict>\n"
		"</plist>\n"
	)
	add_custom_command(
				TARGET CADABRA_PLATFORM_ACE POST_BUILD
				COMMAND codesign ARGS
				--force
				--sign "-"
				--entitlements ${ENTITLEMENTS_PLIST}
				$<TARGET_FILE:CADABRA_PLATFORM_ACE>
	)
endfunction()

function(sign_elf_executable_binary file)
endfunction()
