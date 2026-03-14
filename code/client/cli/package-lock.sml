Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '2', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '2', Tool: '0' }
	}
	'C++': {
		'Monitor.Host': { Version: '../../monitor/host/', Build: '0', Tool: '0' }
		'Monitor.Shared': { Version: '../../monitor/shared/', Build: '0', Tool: '0' }
		'Soup.Core': { Version: '../core/', Build: '1', Tool: '0' }
		'Soup.Generate': { Version: '../../generate/', Build: '0', Tool: '0' }
		'mwasplund|Detours': { Version: 4.0.14, Digest: 'sha256:21c5dcdc2b06bf8fdfb998035130d27debf060046e4fa0ed464a6d8aed3ba234', Build: '0', Tool: '0' }
		'mwasplund|cryptopp': { Version: 1.2.7, Digest: 'sha256:2590a7d8bc1a176c34c2b0ca77e016ecb2b49c156a3bf095a47f5a8063035c78', Build: '0', Tool: '0' }
		'mwasplund|opal': { Version: 0.12.4, Digest: 'sha256:955cd93d5674b85346f21b517019f83b17463ed8f114402eb49134ea67054910', Build: '0', Tool: '0' }
		'mwasplund|reflex': { Version: 5.5.2, Digest: 'sha256:1b49b3cd0ff90bd4f9241f0d00464e8de92dc0454301b0c443297dfe02fac3e6', Build: '0', Tool: '0' }
		'mwasplund|soup-test-assert': { Version: 0.5.0, Digest: 'sha256:ae069ae6775ed31b5eb3ba5c05f33fd8e786cb77884991ae642b887cf7e0dde0', Build: '0', Tool: '0' }
		soup: { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.16.3
				Digest: 'sha256:c0458565184b8a3051c6f638c60fe55be121d45f451726d0c3b79151973c6c70'
				Artifacts: {
					Linux: 'sha256:e7001b58a2a2b9b9cb8c8579f80b06bf5bf6c8f0ebd6df5b6906085b03bcfa43'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|cpp': {
				Version: 0.16.3
				Digest: 'sha256:c0458565184b8a3051c6f638c60fe55be121d45f451726d0c3b79151973c6c70'
				Artifacts: {
					Linux: 'sha256:e7001b58a2a2b9b9cb8c8579f80b06bf5bf6c8f0ebd6df5b6906085b03bcfa43'
				}
			}
			'mwasplund|soup-test-cpp': {
				Version: 0.15.2
				Digest: 'sha256:75b5e9d0a88ab2305f143b0917613ba9f407ee31ac4db595e36f533e1763c6a2'
			}
		}
	}
	'2': {
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
			'mwasplund|parse-modules': {
				Version: 1.2.1
				Digest: 'sha256:30055cb849a20d3b6f0ec7c463a753e4854555860161ad1a0939e4e8c99da523'
				Artifacts: {
					Linux: 'sha256:70e0488a337429bcac6da222bb7483396e8248a3b666f7b49b258fb31be92dc6'
				}
			}
		}
	}
}