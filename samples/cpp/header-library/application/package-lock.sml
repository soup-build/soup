Version: 6
Closure: {
	'C++': {
		'samples-cpp-header-library-application': { Version: './', Build: '0', Tool: '0' }
		'samples-cpp-header-library-library': { Version: '../library/', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.20.1
				Digest: 'sha256:03c5b657804b220628f08a2411130e32289ada0375c475928e64ca5d89a0905a'
				Artifacts: {
					Linux: 'sha256:4737f68f448976de1501556f7e95a2c92c1c6f1ab067c8d369e71d8c96a6cb2e'
					Windows: 'sha256:44ee8107fc669e21753e508caa4c62faae8f5cd46d8126dd4559446b7f7d1e23'
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
				Version: 4.0.0
				Digest: 'sha256:85318f523aac6635109a51f7c0bddcc44fff28da4b7613514db6cefb3ca8ceee'
				Artifacts: {
					Linux: 'sha256:a1d15fada2827293862377a381427c5e75cbca16afcf913e9fcef6346b087a75'
					Windows: 'sha256:ad30416a9a2dc0eebed727e16c8676ae35c3496a3cbe218885af868df4f55d50'
				}
			}
		}
	}
}