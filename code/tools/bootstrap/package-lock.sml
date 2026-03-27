Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '4', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '4', Tool: '0' }
	}
	'C#': {
		'soup-bootstrap': { Version: './', Build: '0', Tool: '0' }
		'soup-native-interop': { Version: '../../generate-sharp/soup-native-interop/', Build: '1', Tool: '0' }
		'soup-utilities': { Version: '../../generate-sharp/utilities/', Build: '1', Tool: '0' }
		opal: { Version: '../../generate-sharp/opal/', Build: '0', Tool: '0' }
	}
	'C++': {
		'monitor-host': { Version: '../../monitor/host/', Build: '2', Tool: '1' }
		'monitor-shared': { Version: '../../monitor/shared/', Build: '2', Tool: '1' }
		'soup-core': { Version: '../../client/core/', Build: '3', Tool: '1' }
		'soup-native': { Version: '../../client/native/', Build: '2', Tool: '1' }
		'mwasplund|detours': { Version: 4.0.14, Digest: 'sha256:21c5dcdc2b06bf8fdfb998035130d27debf060046e4fa0ed464a6d8aed3ba234', Build: '2', Tool: '1' }
		'mwasplund|cryptopp': { Version: 1.2.8, Digest: 'sha256:6d19806db25880437c805a616a3470ddb1b86cba2e6bfd83a378118e1f46e037', Build: '2', Tool: '1' }
		'mwasplund|json11': { Version: 1.1.6, Digest: 'sha256:c6b0981921f926b73e9512d068efde6eb2c5183f6b3a8442bddc67f847d2bfc4', Build: '2', Tool: '1' }
		'mwasplund|opal': { Version: 0.12.6, Digest: 'sha256:eba7621f545cc3f679a72dd034b046e2d0307025342debd9a798f0bc7a45db5b', Build: '2', Tool: '1' }
		'mwasplund|reflex': { Version: 5.5.4, Digest: 'sha256:09e879b8c200c6415543686cf81c710712d07286753cdcd77cec08eab8e81bbe', Build: '2', Tool: '1' }
		'mwasplund|soup-test-assert': { Version: 0.5.0, Digest: 'sha256:ae069ae6775ed31b5eb3ba5c05f33fd8e786cb77884991ae642b887cf7e0dde0', Build: '2', Tool: '1' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|csharp': {
				Version: 0.17.0
				Digest: 'sha256:1099f12280724c982fb22b617d0b59e80eca0965ff312aca37d35ef50633fb27'
				Artifacts: {
					Windows: 'sha256:e6855a2ebf3d7dbc0c359b6fc59263598f36c34fa31a6f5e3ab2bdef384a8be7'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|csharp': {
				Version: 0.17.0
				Digest: 'sha256:1099f12280724c982fb22b617d0b59e80eca0965ff312aca37d35ef50633fb27'
				Artifacts: {
					Windows: 'sha256:e6855a2ebf3d7dbc0c359b6fc59263598f36c34fa31a6f5e3ab2bdef384a8be7'
				}
			}
			'soup|csharp-nuget': {
				Version: 0.7.0
				Digest: 'sha256:0b5bb6797a2ff4e7fbefd6ef9c650f7baa4834dc4743589d30637841fc15511b'
			}
		}
	}
	'2': {
		Wren: {
			'soup|cpp': {
				Version: 0.16.4
				Digest: 'sha256:5b0ddace2b15dddd8b3135b1a55d44da5a5b341db64768887942bd1c9185d7ff'
				Artifacts: {
					Linux: 'sha256:29321540a556729e3a8f2485442dbbf6f00232124c748b08b03f369a24c75f65'
					Windows: 'sha256:ffc24c64947947a7418a38a2cc93fd48475c9e50e1e1b890e5567e3d2fc6d138'
				}
			}
		}
	}
	'3': {
		Wren: {
			'soup|cpp': {
				Version: 0.16.4
				Digest: 'sha256:5b0ddace2b15dddd8b3135b1a55d44da5a5b341db64768887942bd1c9185d7ff'
				Artifacts: {
					Linux: 'sha256:29321540a556729e3a8f2485442dbbf6f00232124c748b08b03f369a24c75f65'
					Windows: 'sha256:ffc24c64947947a7418a38a2cc93fd48475c9e50e1e1b890e5567e3d2fc6d138'
				}
			}
			'mwasplund|soup-test-cpp': {
				Version: 0.15.3
				Digest: 'sha256:2c78af74d69b828d0ff0841d45183321576e5c38332f39bff4b753b7bb692ab0'
			}
		}
	}
	'4': {
		Wren: {
			'soup|c': {
				Version: 0.6.1
				Digest: 'sha256:5e0997c70030a04ea9fec123125f111cd51aee55861553fef8319f4742df7649'
				Artifacts: {
					Linux: 'sha256:9f4c6505fe79d0e46fa07ae9972ec2a405700b709b7b2de1f4caa269a47034df'
					Windows: 'sha256:93f6c11fbb787cb9ad74300e6eafe379124b67608b45219cdf567dba9ed27570'
				}
			}
		}
	}
}
Tools: {
	'0': {
		'C++': {
			'mwasplund|copy': {
				Version: 1.2.0
				Digest: 'sha256:d493afdc0eba473a7f5a544cc196476a105556210bc18bd6c1ecfff81ba07290'
				Artifacts: {
					Linux: 'sha256:cd2e05f53f8e6515383c6b5b5dc6423bda03ee9d4efe7bd2fa74f447495471d2'
					Windows: 'sha256:c4dc68326a11a704d568052e1ed46bdb3865db8d12b7d6d3e8e8d8d6d3fad6c8'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:b423f7173bb4eb233143f6ca7588955a4c4915f84945db5fb06ba2eec3901352'
				Artifacts: {
					Linux: 'sha256:bbf3cd98e44319844de6e9f21de269adeb0dabf1429accad9be97f3bd6c56bbd'
					Windows: 'sha256:4d43a781ed25ae9a97fa6881da7c24425a3162703df19964d987fb2c7ae46ae3'
				}
			}
		}
	}
	'1': {
		'C++': {
			'mwasplund|copy': {
				Version: 1.2.0
				Digest: 'sha256:d493afdc0eba473a7f5a544cc196476a105556210bc18bd6c1ecfff81ba07290'
				Artifacts: {
					Linux: 'sha256:cd2e05f53f8e6515383c6b5b5dc6423bda03ee9d4efe7bd2fa74f447495471d2'
					Windows: 'sha256:c4dc68326a11a704d568052e1ed46bdb3865db8d12b7d6d3e8e8d8d6d3fad6c8'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:b423f7173bb4eb233143f6ca7588955a4c4915f84945db5fb06ba2eec3901352'
				Artifacts: {
					Linux: 'sha256:bbf3cd98e44319844de6e9f21de269adeb0dabf1429accad9be97f3bd6c56bbd'
					Windows: 'sha256:4d43a781ed25ae9a97fa6881da7c24425a3162703df19964d987fb2c7ae46ae3'
				}
			}
			'mwasplund|parse-modules': {
				Version: 1.2.1
				Digest: 'sha256:30055cb849a20d3b6f0ec7c463a753e4854555860161ad1a0939e4e8c99da523'
				Artifacts: {
					Linux: 'sha256:70e0488a337429bcac6da222bb7483396e8248a3b666f7b49b258fb31be92dc6'
					Windows: 'sha256:39db272141c06a4a1e62ddac56e294afd3a9b9c983d2a00d94f7ce815b3fb50e'
				}
			}
		}
	}
}