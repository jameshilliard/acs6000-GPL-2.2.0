cn_tree = {
	'bootconf': {
		'wdt': {
			'get:': '',
			'set:': '',
			'help:': '\'enable/disable watchdog timer (yes/no)\''
		},
		'ip': {
			'get:': '',
			'set:': '',
			'help:': '\'initial ip address\'',
			'len:': '32'
		},
		'bootcmd': {
			'get:': '',
			'set:': '',
			'help:': '\'boot from network or built-in flash (net_boot or hw_boot)\''
		},
		'bootfile': {
			'get:': '',
			'set:': '',
			'help:': '\'name of the boot file on the server\'',
			'len:': '100'
		},
		'image': {
			'get:': '',
			'set:': '',
			'help:': '\'the image number to use (1 or 2)\''
		},
		'fallback': {
			'get:': '',
			'set:': '',
			'help:': '\'fallback to other boot modes (yes or no)\''
		},
		'serverip': {
			'get:': '',
			'set:': '',
			'help:': '\'the ip address of the tftp server\'',
			'len:': '32'
		},
		'fecmode1': {
			'get:': '',
			'set:': '',
			'help:': '\'auto, 100F, 100H, 10F, 10H\''
		},
		'serial#': {
			'get:': '',
			'help:': '\'Appliance serial number\''
		},
		'ethaddr': {
			'get:': '',
			'help:': '\'Eth0 MAC address\''
		},
		'fecmode2': {
			'get:': '',
			'set:': '',
			'help:': '\'auto, 100F, 100H, 10F, 10H\''
		},
		'baudrate': {
			'get:': '',
			'set:': '',
			'help:': '\'initial baud rate of the console\''
		}
	},
	'multi-edit': {
		'elemLst': {
			'get:': '',
			'set:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'paramLst': {
			'get:': '',
			'set:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'valueLst': {
			'get:': '',
			'set:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		}
	},
	'sysvinit': {
		'service': {
			'apache2': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'hotplug': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'ipsec': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'logpipe': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'ntpclient': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'portmap': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'snmpd': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'sshd': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'syslog-ng': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'udev': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'xinetd': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			},
			'ypbind': {
				'enable': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				}
			}
		}
	},
	'network': {
		'interface': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'len:': '64',
			'*': {
				'active': {
					'get:': '',
					'set:': '',
					'help:': '"enable or disable $1 with yes or no"',
					'type:': 'boolean'
				},
				'family': {
					'get:': ''
				},
				'method': {
					'get:': '',
					'set:': '',
					'help:': 'if(equal($1, \'lo\'), \'must be loopback\', \'static or dhcp\')',
					'type:': 'interface4Config'
				},
				'address': {
					'get:': '',
					'set:': '',
					'help:': '"ip address for interface $1"',
					'type:': 'ipv4Address',
					'len:': '15'
				},
				'netmask': {
					'get:': '',
					'set:': '',
					'help:': '"netmask for interface $1"',
					'type:': 'ipv4Address',
					'len:': '15'
				},
				'broadcast': {
					'get:': '',
					'set:': '',
					'help:': '"broadcast address for interface $1"',
					'type:': 'ipv4Address',
					'len:': '15'
				},
				'gateway': {
					'get:': '',
					'set:': '',
					'help:': '"the default gateway address, or blank for no gateway"',
					'type:': 'ipAddress'
				},
				'mtu': {
					'get:': '',
					'set:': '',
					'help:': '"maximum transfer unit, number"',
					'type:': 'int',
					'len:': '10'
				},
				'method6': {
					'get:': '',
					'set:': '',
					'type:': 'interface6Config'
				},
				'address6': {
					'get:': '',
					'set:': '',
					'help:': '"IPv6 address for interface $1"',
					'type:': 'ipv6Address',
					'len:': '39'
				},
				'netmask6': {
					'get:': '',
					'set:': '',
					'help:': '"netmask for interface $1"',
					'type:': 'int',
					'len:': '10'
				},
				'gateway6': {
					'get:': '',
					'set:': '',
					'help:': '"the default gateway address, or blank for no gateway"',
					'type:': 'ipAddress'
				},
				'mtu6': {
					'get:': '',
					'set:': '',
					'help:': '"maximum transfer unit, number"',
					'type:': 'int',
					'len:': '10'
				},
				'wireless': {
					'essid': {
						'get:': '',
						'set:': '',
						'help:': '\'the wireless network name\'',
						'len:': '32'
					},
					'mode': {
						'get:': '',
						'set:': '',
						'help:': '"the wireless mode (ad-hoc or managed)"'
					},
					'channel': {
						'get:': '',
						'set:': ''
					},
					'key': {
						'get:': '',
						'set:': '',
						'help:': '"the WEP key"',
						'len:': '26'
					}
				},
				'alias': {
					'add:': '',
					'del:': '',
					'move:': '',
					'list:': '',
					'len:': '32',
					'*': {
						'address': {
							'get:': '',
							'set:': '',
							'help:': '"ip address for interface $ifname"',
							'type:': 'ipv4Address',
							'len:': '15'
						},
						'netmask': {
							'get:': '',
							'set:': '',
							'help:': '"netmask for interface $ifname"',
							'type:': 'ipv4Address',
							'len:': '15'
						},
						'broadcast': {
							'get:': '',
							'set:': '',
							'help:': '"broadcast address for interface $ifname"',
							'type:': 'ipv4Address',
							'len:': '15'
						}
					}
				}
			}
		},
		'bonding': {
			'active': {
				'get:': '',
				'set:': '',
				'help:': '\'set to yes or no. Enables or disables the interface bond0\'',
				'type:': 'boolean'
			},
			'slaves': {
				'get:': '',
				'set:': '',
				'help:': '"list of interfaces bonded"',
				'type:': 'UTF8String',
				'len:': '4000'
			},
			'mode': {
				'get:': '',
				'set:': '',
				'help:': '"Bonding mode"',
				'type:': 'int',
				'len:': '10'
			},
			'miimon': {
				'get:': '',
				'set:': '',
				'help:': '"MII monitoring period"',
				'type:': 'int',
				'len:': '10'
			},
			'updelay': {
				'get:': '',
				'set:': '',
				'help:': '"Link up delay"',
				'type:': 'int',
				'len:': '10'
			}
		},
		'hostname': {
			'get:': '',
			'set:': '',
			'help:': '\'the host name\'',
			'len:': '63'
		},
		'resolv': {
			'dns0': {
				'get:': '',
				'set:': '',
				'help:': '\'the ip address of the primary DNS server\'',
				'type:': 'ipAddress'
			},
			'dns1': {
				'get:': '',
				'set:': '',
				'help:': '\'the ip address of the secondary DNS server, or blank for no secondary DNS server\'',
				'type:': 'ipAddress'
			},
			'domain': {
				'get:': '',
				'set:': '',
				'help:': '\'the default domain name\'',
				'len:': '255'
			}
		},
		'hosts': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'help:': '\'list of hosts, indexed by IP address\'',
			'type:': 'ipAddress',
			'*': {
				'name': {
					'get:': '',
					'set:': '',
					'help:': '"the host name for IP address $1"',
					'len:': '255'
				},
				'alias': {
					'get:': '',
					'set:': '',
					'help:': '"an optional alias name for IP address $1"',
					'len:': '255'
				}
			}
		},
		'st_routes': {
			'add:': '',
			'del:': '',
			'list:': '',
			'help:': '\'list of targets (networks in the form 1.2.3.0/24 or host IPs, or default)\'',
			'*': {
				'gateway': {
					'get:': '',
					'set:': '',
					'help:': '\'set to an ip address or empty\'',
					'type:': 'ipv4Address',
					'len:': '15'
				},
				'device': {
					'get:': '',
					'set:': '',
					'help:': '\'set to an interface name or empty\''
				},
				'destination': {
					'get:': '',
					'set:': '',
					'help:': '\'a number, usually 0\''
				},
				'metric': {
					'get:': '',
					'set:': '',
					'help:': '\'a number, usually 0\'',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'st6_routes': {
			'add:': '',
			'del:': '',
			'list:': '',
			'help:': '\'list of targets (networks in the form netaddr/prefixlen or host IPs, or default)\'',
			'*': {
				'gateway': {
					'get:': '',
					'set:': '',
					'help:': '\'set to an ip address or empty\'',
					'type:': 'ipv6Address',
					'len:': '39'
				},
				'device': {
					'get:': '',
					'set:': '',
					'help:': '\'set to an interface name or empty\''
				},
				'destination': {
					'get:': '',
					'set:': '',
					'help:': '\'a number, usually 0\''
				},
				'metric': {
					'get:': '',
					'set:': '',
					'help:': '\'a number, usually 0\'',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'ipv4': {
			'icmp': {
				'echo_ignore_all': {
					'get:': '',
					'set:': '',
					'help:': '\'enable or disable icmp echo replies. Set to yes or no\'',
					'type:': 'boolean'
				}
			},
			'ip': {
				'forward': {
					'get:': '',
					'set:': '',
					'help:': '\'enable or disable forwarding. Set to yes or no\'',
					'type:': 'boolean'
				}
			}
		},
		'ipv6': {
			'enable': {
				'get:': '',
				'set:': '',
				'help:': '\'enable or disable ipv6 stack. Set to yes or no\'',
				'type:': 'boolean'
			}
		},
		'dhcpv6-option': {
			'dns': {
				'get:': '',
				'set:': '',
				'help:': '\'enable DNS configuration via DHCPv6\'',
				'type:': 'boolean'
			},
			'domain': {
				'get:': '',
				'set:': '',
				'help:': '\'enable domain configuration via DHCPv6\'',
				'type:': 'boolean'
			}
		}
	},
	'xinetd': {
		'service': {
			'list:': '',
			'*': {
				'enable': {
					'get:': '',
					'set:': '',
					'help:': '"enable $1 with yes, disable with \'no\'"',
					'type:': 'boolean'
				}
			}
		}
	},
	'authorization': {
		'access': {
			'id': {
				'add:': '',
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'*': {
					'type': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'configuration': {
						'add:': '',
						'del:': '',
						'list:': '',
						'type:': 'UTF8String',
						'len:': '4000',
						'*': {
							'permission': {
								'add:': '',
								'del:': '',
								'list:': '',
								'type:': 'UTF8String',
								'len:': '4000',
								'*': {
								}
							}
						}
					},
					'management': {
						'add:': '',
						'del:': '',
						'list:': '',
						'type:': 'UTF8String',
						'len:': '4000',
						'*': {
							'permission': {
								'add:': '',
								'del:': '',
								'list:': '',
								'type:': 'UTF8String',
								'len:': '4000',
								'*': {
								}
							}
						}
					}
				}
			},
			'name': {
				'add:': '',
				'del:': '',
				'list:': '',
				'*': {
					'id': {
						'get:': ''
					},
					'configuration': {
						'add:': '',
						'del:': '',
						'list:': '',
						'*': {
							'list:': '',
							'permission': {
								'add:': '',
								'del:': ''
							}
						}
					},
					'management': {
						'add:': '',
						'del:': '',
						'list:': '',
						'*': {
							'list:': '',
							'permission': {
								'add:': '',
								'del:': ''
							}
						}
					}
				}
			},
			'pdu_name': {
				'move:': ''
			}
		},
		'group': {
			'id': {
				'add:': '',
				'del:': '',
				'list:': '',
				'*': {
					'name': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'type': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'member': {
						'add:': '',
						'del:': '',
						'list:': '',
						'type:': 'UTF8String',
						'len:': '30',
						'*': {
							'get:': '',
							'set:': ''
						}
					}
				}
			},
			'name': {
				'add:': '',
				'del:': '',
				'list:': '',
				'len:': '30',
				'*': {
					'id': {
						'get:': ''
					},
					'member': {
						'add:': '',
						'del:': '',
						'list:': '',
						'type:': 'UTF8String',
						'len:': '30',
						'*': {
						}
					}
				}
			}
		}
	},
	'target': {
		'rid': {
			'get:': '',
			'set:': ''
		},
		'id': {
			'add:': '',
			'del:': '',
			'list:': '',
			'*': {
				'name': {
					'get:': '',
					'set:': ''
				},
				'associations': {
					'add:': '',
					'del:': '',
					'list:': '',
					'*': {
						'type': {
							'get:': '',
							'set:': ''
						}
					}
				},
				'outlets': {
					'list:': ''
				}
			}
		},
		'name': {
			'add:': '',
			'del:': '',
			'list:': '',
			'len:': '100',
			'*': {
				'list:': '',
				'id': {
					'get:': ''
				},
				'*': {
					'get:': '',
					'set:': '',
					'add:': '',
					'del:': '',
					'list:': '',
					'*': {
						'get:': '',
						'set:': '',
						'list:': '',
						'*': {
							'get:': '',
							'set:': ''
						}
					}
				}
			}
		},
		'outlet_group': {
			'list:': '',
			'*': {
				'pdu': {
					'del:': '',
					'list:': '',
					'*': {
						'outlet': {
							'add:': '',
							'del:': '',
							'list:': '',
							'type:': 'int',
							'len:': '10'
						},
						'outlet_range': {
							'get:': '',
							'set:': '',
							'add:': '',
							'del:': '',
							'len:': '64'
						}
					}
				}
			}
		}
	},
	'adsap2': {
		'daemon': {
			'enabled': {
				'get:': '',
				'set:': '',
				'help:': '"Should the ADSAP2 daemon be listening for any requests"',
				'type:': 'boolean'
			}
		},
		'auth_servers': {
			'1': {
				'get:': '',
				'set:': '',
				'help:': '"An ADSAP2 Auth Server"',
				'len:': '100'
			},
			'2': {
				'get:': '',
				'set:': '',
				'help:': '"An ADSAP2 Auth Server"',
				'len:': '100'
			},
			'3': {
				'get:': '',
				'set:': '',
				'help:': '"An ADSAP2 Auth Server"',
				'len:': '100'
			},
			'4': {
				'get:': '',
				'set:': '',
				'help:': '"An ADSAP2 Auth Server"',
				'len:': '100'
			}
		},
		'security': {
			'mode': {
				'get:': '',
				'set:': '',
				'help:': '"Is the device in secure or non-secure mode"'
			},
			'local_cred': {
				'key': {
					'path': {
						'get:': '',
						'help:': '"The location of the PEM file containing the active local private key credential"'
					},
					'pem': {
						'get:': '',
						'set:': '',
						'help:': '"The contents of the active local private key credential file"'
					}
				},
				'x509': {
					'path': {
						'get:': '',
						'help:': '"The location of the PEM file containing the active local certificate credential"'
					},
					'pem': {
						'get:': '',
						'set:': '',
						'help:': '"The contents of the active local certificate credential file"'
					}
				}
			},
			'remote_cred': {
				'x509': {
					'path': {
						'get:': '',
						'help:': '"The location of the PEM file containing the active local certificate credential"'
					},
					'pem': {
						'get:': '',
						'set:': '',
						'help:': '"The contents of the active local certificate credential file"'
					}
				}
			}
		},
		'rights': {
			'appliance_admin': {
				'appliance_rights': {
					'get:': ''
				},
				'target_rights': {
					'get:': ''
				}
			},
			'user_admin': {
				'appliance_rights': {
					'get:': ''
				},
				'target_rights': {
					'get:': ''
				}
			},
			'user': {
				'appliance_rights': {
					'get:': ''
				},
				'target_rights': {
					'get:': ''
				}
			}
		}
	},
	'aidp': {
		'daemon': {
			'enabled': {
				'get:': '',
				'set:': '',
				'help:': '"Should the AIDP daemon be listening for any requests"',
				'type:': 'boolean'
			}
		},
		'discovery': {
			'appliance_type': {
				'get:': '',
				'set:': '',
				'help:': '"The appliance type the AIDP should report upon discovery"',
				'type:': 'int',
				'len:': '10'
			}
		},
		'network': {
			'ifname': {
				'get:': '',
				'set:': ''
			},
			'allow_config': {
				'get:': '',
				'set:': '',
				'help:': '"Should the AIDP daemon accept network configuration requests"',
				'type:': 'boolean'
			}
		}
	},
	'apache2': {
		'http_enabled': {
			'get:': '',
			'set:': '',
			'help:': '\'http/https enabled (yes or no)\'',
			'type:': 'boolean'
		},
		'https_enabled': {
			'get:': '',
			'set:': '',
			'help:': '\'http/https enabled (yes or no)\'',
			'type:': 'boolean'
		},
		'https_dsview_enabled': {
			'get:': '',
			'set:': '',
			'help:': '\'https connection with dsview enabled (yes or no)\'',
			'type:': 'boolean'
		},
		'https_avworks_enabled': {
			'get:': '',
			'set:': '',
			'help:': '\'https connection with avworks enabled (yes or no)\'',
			'type:': 'boolean'
		},
		'http_port': {
			'get:': '',
			'set:': '',
			'help:': '\'the port number\'',
			'type:': 'int',
			'len:': '10'
		},
		'https_port': {
			'get:': '',
			'set:': '',
			'help:': '\'the port number\'',
			'type:': 'int',
			'len:': '10'
		},
		'https_dsview_port': {
			'get:': '',
			'set:': '',
			'help:': '\'the port number\'',
			'type:': 'int',
			'len:': '10'
		},
		'https_ssl_version': {
			'get:': '',
			'set:': '',
			'help:': '\'SSL protocol version for https. comma separated list of 2 and 3\''
		},
		'http_redirect_to_https': {
			'get:': '',
			'set:': '',
			'help:': '\'redirect to https (yes or no)\'',
			'type:': 'boolean'
		},
		'https_ssl_cipher': {
			'get:': '',
			'set:': '',
			'help:': '\'SSL ciphersuite for https. comma separated list of High and Medium\''
		}
	},
	'appliance': {
		'help-url': {
			'get:': '',
			'set:': '',
			'help:': '\'the url for online help\'',
			'len:': '256'
		},
		'language': {
			'get:': '',
			'set:': '',
			'help:': '\'the language\'',
			'len:': '32'
		},
		'languages': {
			'get:': '',
			'set:': '',
			'help:': '\'the list of supported languages\'',
			'len:': '256'
		},
		'dla': {
			'state': {
				'get:': '',
				'set:': '',
				'help:': '\'dla setting (yes/no)\'',
				'type:': 'boolean'
			},
			'type': {
				'get:': '',
				'set:': '',
				'type:': 'dataBufType'
			},
			'timestamp': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			},
			'alert-enable': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			},
			'alert-string': {
				'list:': '',
				'type:': 'int',
				'len:': '10',
				'*': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '64'
				}
			}
		},
		'idle-timeout': {
			'get:': '',
			'set:': '',
			'help:': '\'the idle timeout in seconds\'',
			'type:': 'int',
			'len:': '5'
		},
		'serial-ports-require': {
			'authorization': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			}
		},
		'pluggable-device': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'first-time-config': {
			'required': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			}
		},
		'outlet-require': {
			'authorization': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			}
		},
		'issue': {
			'get:': '',
			'set:': '',
			'help:': '\'the issue file\'',
			'len:': '63'
		},
		'vspc-port': {
			'get:': '',
			'set:': '',
			'help:': '\'vspc port to configure vm\'',
			'type:': 'int',
			'len:': '4'
		}
	},
	'auth': {
		'type': {
			'get:': '',
			'set:': '',
			'help:': 'if(equal($0, \'type-ppp\'), \'the ppp authentication type (for the auxport modem and pcmcia/cardbus modems)\', \'the global (unit) authentication type\')'
		},
		'type-ppp': {
			'get:': '',
			'set:': '',
			'help:': 'if(equal($0, \'type-ppp\'), \'the ppp authentication type (for the auxport modem and pcmcia/cardbus modems)\', \'the global (unit) authentication type\')'
		},
		'single-sign-on': {
			'get:': '',
			'set:': '',
			'help:': '\'single sign on enable (yes/no)\'',
			'type:': 'boolean'
		},
		'single-sign-on-type': {
			'get:': '',
			'set:': '',
			'help:': '\'single sign on type\''
		},
		'nis': {
			'domainname': {
				'get:': '',
				'set:': '',
				'help:': '\'the NIS domain name\'',
				'len:': '100'
			},
			'server': {
				'get:': '',
				'set:': '',
				'help:': '\'the NIS server, or broadcast\'',
				'len:': '100'
			}
		},
		'radius': {
			'authhost1': {
				'get:': '',
				'set:': '',
				'help:': 'if(equal(substr($0, \'4\'), \'auth\'), \'the authentication server\', \'the accounting server\')',
				'len:': '100'
			},
			'authhost2': {
				'get:': '',
				'set:': '',
				'help:': 'if(equal(substr($0, \'4\'), \'auth\'), \'the authentication server\', \'the accounting server\')',
				'len:': '100'
			},
			'accthost1': {
				'get:': '',
				'set:': '',
				'help:': 'if(equal(substr($0, \'4\'), \'auth\'), \'the authentication server\', \'the accounting server\')',
				'len:': '100'
			},
			'accthost2': {
				'get:': '',
				'set:': '',
				'help:': 'if(equal(substr($0, \'4\'), \'auth\'), \'the authentication server\', \'the accounting server\')',
				'len:': '100'
			},
			'secret': {
				'get:': '',
				'set:': '',
				'help:': 'option(index($_, \'secret\', \'timeout\', \'retries\'), \'the radius secret\', \'the timout in seconds\', \'the number of retries before failure\')'
			},
			'timeout': {
				'get:': '',
				'set:': '',
				'help:': 'option(index($_, \'secret\', \'timeout\', \'retries\'), \'the radius secret\', \'the timout in seconds\', \'the number of retries before failure\')'
			},
			'retries': {
				'get:': '',
				'set:': '',
				'help:': 'option(index($_, \'secret\', \'timeout\', \'retries\'), \'the radius secret\', \'the timout in seconds\', \'the number of retries before failure\')'
			},
			'servicetype-enable': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			}
		},
		'tacplus': {
			'authhost1': {
				'get:': '',
				'set:': '',
				'len:': '100'
			},
			'authhost2': {
				'get:': '',
				'set:': '',
				'len:': '100'
			},
			'accthost1': {
				'get:': '',
				'set:': '',
				'len:': '100'
			},
			'accthost2': {
				'get:': '',
				'set:': '',
				'len:': '100'
			},
			'secret': {
				'get:': '',
				'set:': '',
				'help:': '\'the shared secret\'',
				'len:': '64'
			},
			'retries': {
				'get:': '',
				'set:': '',
				'help:': '\'number of retries before failure\'',
				'type:': 'int',
				'len:': '5'
			},
			'timeout': {
				'get:': '',
				'set:': '',
				'help:': '\'timeout in seconds before failure\'',
				'type:': 'int',
				'len:': '5'
			},
			'service': {
				'get:': '',
				'set:': '',
				'help:': '\'ppp or raccess or shell\''
			},
			'tacacs_vers': {
				'get:': '',
				'set:': '',
				'type:': 'UTF8String',
				'len:': '4000'
			},
			'userlevel-enable': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			}
		},
		'krb5': {
			'realm': {
				'get:': '',
				'set:': '',
				'help:': '\'the kerberos realm to use\'',
				'len:': '100'
			},
			'server': {
				'get:': '',
				'set:': '',
				'help:': '\'ip of the kerberos server\'',
				'len:': '100'
			},
			'domains': {
				'get:': '',
				'set:': '',
				'len:': '100'
			}
		},
		'ldap': {
			'host': {
				'get:': '',
				'set:': '',
				'len:': '100'
			},
			'base': {
				'get:': '',
				'set:': '',
				'len:': '100'
			},
			'binddn': {
				'get:': '',
				'set:': '',
				'len:': '70'
			},
			'bindpw': {
				'get:': '',
				'set:': '',
				'help:': '"the ldap password, or empty"',
				'len:': '64'
			},
			'pam_login_attribute': {
				'get:': '',
				'set:': '',
				'len:': '64'
			},
			'ssl': {
				'get:': '',
				'set:': '',
				'help:': '\'whether to use ssl. Set to on, off or start_tls\''
			}
		},
		'smb': {
			'domain': {
				'get:': '',
				'set:': '',
				'len:': '63'
			},
			'server1': {
				'get:': '',
				'set:': ''
			},
			'server2': {
				'get:': '',
				'set:': ''
			}
		}
	},
	'connection': {
		'power': {
			'pdu_id': {
				'add:': '',
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'*': {
					'pdu_name': {
						'get:': ''
					},
					'target_id': {
						'list:': ''
					}
				}
			},
			'outlet_index': {
				'list:': '',
				'*': {
					'outlet_id': {
						'get:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			},
			'outlet_id': {
				'add:': '',
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'*': {
					'pdu_id': {
						'get:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'number': {
						'get:': '',
						'type:': 'int',
						'len:': '10'
					},
					'target_id': {
						'list:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'pdu_name': {
						'get:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			},
			'pdu_name': {
				'add:': '',
				'del:': '',
				'move:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '16',
				'*': {
					'pdu_name': {
					},
					'id': {
						'get:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'target_id': {
						'list:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'outlet': {
						'add:': '',
						'del:': '',
						'list:': '',
						'type:': 'int',
						'len:': '10',
						'*': {
							'id': {
								'get:': '',
								'type:': 'UTF8String',
								'len:': '4000'
							},
							'target_id': {
								'get:': '',
								'list:': '',
								'type:': 'UTF8String',
								'len:': '4000'
							}
						}
					}
				}
			}
		},
		'serial': {
			'name': {
				'add:': '',
				'del:': '',
				'list:': '',
				'*': {
					'list:': '',
					'id': {
						'get:': ''
					},
					'*': {
						'get:': '',
						'set:': '',
						'mset': {
							'set:': ''
						}
					}
				}
			},
			'alert-string-total': {
				'get:': '',
				'set:': '',
				'type:': 'int',
				'len:': '10'
			},
			'id': {
				'add:': '',
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'all': {
					'protocol': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'authentication': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'data-buffering': {
						'state': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'type': {
							'get:': '',
							'set:': '',
							'type:': 'dataBufType'
						},
						'timestamp': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'notify-login': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'logging': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						}
					}
				},
				'*': {
					'mset': {
						'set:': ''
					},
					'target-id': {
						'get:': ''
					},
					'port': {
						'get:': ''
					},
					'name': {
						'get:': '',
						'set:': '',
						'len:': '100'
					},
					'isdefault': {
						'get:': '',
						'type:': 'boolean'
					},
					'protocol': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'authentication': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'break-sequence': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '10'
					},
					'linefeed-suppression': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'null-suppression': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'notify-multisession': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'show-info-msg': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'transmission-interval': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '5'
					},
					'break-interval': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '5'
					},
					'pmk-sequence': {
						'get:': '',
						'set:': '',
						'help:': '\'the power menu hot key\'',
						'len:': '10'
					},
					'escape-sequence': {
						'get:': '',
						'set:': '',
						'help:': '\'the escape sequence\'',
						'len:': '10'
					},
					'data-buffering': {
						'mset': {
							'set:': ''
						},
						'state': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'type': {
							'get:': '',
							'set:': '',
							'type:': 'dataBufType'
						},
						'timestamp': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'notify-login': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'logging': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'alert-enable': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'alert-string': {
							'add:': '',
							'del:': '',
							'list:': '',
							'type:': 'int',
							'len:': '10',
							'madd': {
								'set:': ''
							},
							'mdel': {
								'set:': ''
							},
							'mset': {
								'set:': ''
							},
							'*': {
								'get:': '',
								'set:': '',
								'type:': 'UTF8String',
								'len:': '64',
								'mset': {
									'set:': ''
								}
							}
						}
					}
				}
			},
			'passthrough_cmd': {
				'get:': '',
				'type:': 'UTF8String',
				'len:': '4000'
			}
		},
		'all': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'type': {
					'get:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				}
			}
		}
	},
	'global-data-buffering': {
		'local': {
			'rotation': {
				'spare-segments': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '2'
				},
				'size': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '4'
				}
			}
		},
		'nfs': {
			'server': {
				'get:': '',
				'set:': '',
				'type:': 'UTF8String',
				'len:': '100'
			},
			'path': {
				'get:': '',
				'set:': '',
				'type:': 'UTF8String',
				'len:': '100'
			},
			'rotation': {
				'spare-segments': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '2'
				},
				'size': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '5'
				}
			}
		},
		'syslog': {
			'facility': {
				'get:': '',
				'set:': '',
				'help:': '\'LOG_LOCAL0 to LOG_LOCAL5\''
			}
		},
		'ip': {
			'get:': ''
		}
	},
	'dhcpd': {
		'subnet': {
			'add:': '',
			'del:': '',
			'list:': '',
			'*': {
				'range': {
					'low': {
						'get:': '',
						'set:': ''
					},
					'high': {
						'get:': '',
						'set:': ''
					}
				},
				'host': {
					'add:': '',
					'del:': '',
					'move:': '',
					'list:': '',
					'*': {
						'hwaddr': {
							'get:': '',
							'set:': ''
						},
						'address': {
							'get:': '',
							'set:': ''
						}
					}
				},
				'option': {
					'domain-name': {
						'get:': '',
						'set:': '',
						'len:': '63'
					},
					'domain-name-servers': {
						'get:': '',
						'set:': ''
					},
					'router': {
						'get:': '',
						'set:': ''
					}
				}
			}
		}
	},
	'dialin-secure': {
		'login': {
			'get:': '',
			'set:': ''
		},
		'otp': {
			'get:': '',
			'set:': ''
		},
		'ppp': {
			'get:': '',
			'set:': ''
		},
		'callback': {
			'add:': '',
			'del:': '',
			'list:': '',
			'len:': '30',
			'*': {
				'number': {
					'get:': '',
					'set:': '',
					'len:': '40'
				}
			}
		}
	},
	'port': {
		'serial': {
			'dialin': {
				'add:': '',
				'del:': '',
				'move:': '',
				'list:': '',
				'default': {
					'speed': {
						'get:': '',
						'set:': '',
						'help:': '\'modem speed\'',
						'type:': 'int',
						'len:': '10'
					},
					'initchat': {
						'get:': '',
						'set:': '',
						'help:': '\'modem initialization string (AT commands)\'',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'data-flow': {
						'get:': '',
						'set:': '',
						'help:': '\'flow control, one of none, hard, soft or both\'',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'ppp': {
						'iplocal': {
							'get:': '',
							'set:': '',
							'type:': 'ipv4Address',
							'len:': '15'
						},
						'ipremote': {
							'get:': '',
							'set:': '',
							'type:': 'ipv4Address',
							'len:': '15'
						},
						'ipv6local': {
							'get:': '',
							'set:': '',
							'type:': 'ipv6Address',
							'len:': '39'
						},
						'ipv6remote': {
							'get:': '',
							'set:': '',
							'type:': 'ipv6Address',
							'len:': '39'
						},
						'mtu': {
							'get:': '',
							'set:': '',
							'type:': 'int',
							'len:': '10'
						},
						'mru': {
							'get:': '',
							'set:': '',
							'type:': 'int',
							'len:': '10'
						},
						'auth': {
							'get:': '',
							'set:': '',
							'help:': '\'enable or disable the authentication requirement of the peer (none,PAP,CHAP,EAP)\'',
							'type:': 'UTF8String',
							'len:': '63'
						},
						'chap-interval': {
							'get:': '',
							'set:': '',
							'help:': '\'If this option is given, pppd will rechallenge the peer every n seconds\'',
							'type:': 'int',
							'len:': '10'
						},
						'chap-max-challenge': {
							'get:': '',
							'set:': '',
							'help:': '\'Set the maximum number of CHAP challenge transmissions to <n>\'',
							'type:': 'int',
							'len:': '10'
						},
						'chap-restart': {
							'get:': '',
							'set:': '',
							'help:': '\'Set the CHAP restart interval (retransmission timeout for challenges) to <n> seconds\'',
							'type:': 'int',
							'len:': '10'
						},
						'idle': {
							'get:': '',
							'set:': '',
							'help:': '\'Specifies that pppd should disconnect if the link is idle for n seconds\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				},
				'*': {
					'mset': {
						'set:': ''
					},
					'active': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'name': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'oldstatus': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'speed': {
						'get:': '',
						'set:': '',
						'help:': '\'modem speed\'',
						'type:': 'int',
						'len:': '10'
					},
					'initchat': {
						'get:': '',
						'set:': '',
						'help:': '\'modem initialization string (AT commands)\'',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'data-flow': {
						'get:': '',
						'set:': '',
						'help:': '\'flow control, one of none, hard, soft or both\'',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'ppp': {
						'mset': {
							'set:': ''
						},
						'ppptype': {
							'get:': ''
						},
						'noip': {
							'set:': '',
							'type:': 'boolean'
						},
						'iplocal': {
							'get:': '',
							'set:': '',
							'type:': 'ipv4Address',
							'len:': '15'
						},
						'ipremote': {
							'get:': '',
							'set:': '',
							'type:': 'ipv4Address',
							'len:': '15'
						},
						'ipv6local': {
							'get:': '',
							'set:': '',
							'type:': 'ipv6Address',
							'len:': '39'
						},
						'ipv6remote': {
							'get:': '',
							'set:': '',
							'type:': 'ipv6Address',
							'len:': '39'
						},
						'ipcp': {
							'get:': '',
							'set:': '',
							'type:': 'boolean'
						},
						'mtu': {
							'get:': '',
							'set:': '',
							'type:': 'int',
							'len:': '10'
						},
						'mru': {
							'get:': '',
							'set:': '',
							'type:': 'int',
							'len:': '10'
						},
						'auth': {
							'get:': '',
							'set:': '',
							'help:': '\'enable or disable the authentication requirement of the peer (none,PAP,CHAP,EAP)\'',
							'type:': 'UTF8String',
							'len:': '63',
							'mset': {
								'set:': ''
							},
							'username': {
								'get:': '',
								'set:': '',
								'len:': '32'
							},
							'passphrase': {
								'get:': '',
								'set:': '',
								'len:': '100'
							}
						},
						'chap-interval': {
							'get:': '',
							'set:': '',
							'help:': '\'If this option is given, pppd will rechallenge the peer every n seconds\'',
							'type:': 'int',
							'len:': '10'
						},
						'chap-max-challenge': {
							'get:': '',
							'set:': '',
							'help:': '\'Set the maximum number of CHAP challenge transmissions to <n>\'',
							'type:': 'int',
							'len:': '10'
						},
						'chap-restart': {
							'get:': '',
							'set:': '',
							'help:': '\'Set the CHAP restart interval (retransmission timeout for challenges) to <n> seconds\'',
							'type:': 'int',
							'len:': '10'
						},
						'idle': {
							'get:': '',
							'set:': '',
							'help:': '\'Specifies that pppd should disconnect if the link is idle for n seconds\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				}
			},
			'total_ports': {
				'get:': ''
			},
			'autoanswer': {
				'add:': '',
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'*': {
					'mset': {
						'set:': ''
					},
					'escseq': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '50'
					},
					'response': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '50'
					}
				}
			},
			'autodiscover': {
				'ADtimeout': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '5'
				},
				'probetimeout': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '5'
				},
				'probe': {
					'add:': '',
					'del:': '',
					'list:': '',
					'type:': 'UTF8String',
					'len:': '4000',
					'*': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '100',
						'mset': {
							'set:': ''
						}
					}
				},
				'match': {
					'add:': '',
					'del:': '',
					'list:': '',
					'type:': 'UTF8String',
					'len:': '4000',
					'*': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '100',
						'mset': {
							'set:': ''
						}
					}
				},
				'default-speed': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '10'
				},
				'speed-list': {
					'list:': '',
					'*': {
						'get:': '',
						'set:': '',
						'type:': 'boolean',
						'mset': {
							'set:': ''
						}
					}
				}
			},
			'profile': {
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'all': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'*': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000',
					'mset': {
						'set:': ''
					},
					'clone': {
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			},
			'status': {
				'add:': '',
				'del:': '',
				'move:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'all': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				},
				'*': {
					'get:': '',
					'set:': '',
					'type:': 'boolean',
					'mset': {
						'set:': ''
					}
				}
			},
			'name': {
				'list:': '',
				'type:': 'int',
				'len:': '10',
				'*': {
					'get:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				}
			},
			'port-settings': {
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'all': {
					'pinout': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'speed': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'stopbits': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'parity': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'data-size': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'flow': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'*': {
					'mset': {
						'set:': ''
					},
					'physconf': {
						'get:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'pinout': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'speed': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'stopbits': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'parity': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'data-size': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'flow': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			},
			'pool': {
				'add:': '',
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '64',
				'*': {
					'ipv4add': {
						'get:': '',
						'set:': '',
						'type:': 'ipv4Address',
						'len:': '15'
					},
					'ipv6add': {
						'get:': '',
						'set:': '',
						'type:': 'ipv6Address',
						'len:': '39'
					},
					'ipv4interface': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'ipv6interface': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'tcpportalias': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'ports': {
						'get:': '',
						'set:': '',
						'list:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'id': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					}
				}
			},
			'cas': {
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'enabled': {
					'list:': ''
				},
				'*': {
					'mset': {
						'set:': ''
					},
					'autodiscovery': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'speedautodiscovery': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'autodiscovery-alias': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '100'
					},
					'autoanswer': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'ipv4add': {
						'get:': '',
						'set:': '',
						'type:': 'ipv4Address',
						'len:': '15'
					},
					'ipv6add': {
						'get:': '',
						'set:': '',
						'type:': 'ipv6Address',
						'len:': '39'
					},
					'ipv4interface': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'ipv6interface': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'tcpportalias': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'connection-id': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'dtrmode': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'dtroff-interval': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '5'
					},
					'dcdregards': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					}
				}
			},
			'power': {
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'*': {
					'mset': {
						'set:': ''
					},
					'pdu-type': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'poll-rate': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '5'
					},
					'speed-autodetection': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'buzzer': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'syslog': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'overcurrent': {
						'get:': '',
						'set:': '',
						'type:': 'boolean'
					},
					'cycle-interval': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '5'
					}
				}
			}
		},
		'aux': {
			'profile': {
				'del:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'*': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000',
					'mset': {
						'set:': ''
					},
					'auxtype': {
						'get:': ''
					}
				}
			},
			'status': {
				'add:': '',
				'del:': '',
				'move:': '',
				'list:': '',
				'type:': 'UTF8String',
				'len:': '4000',
				'*': {
					'get:': '',
					'set:': '',
					'type:': 'boolean',
					'mset': {
						'set:': ''
					}
				}
			}
		}
	},
	'event-notify': {
		'snmptrap': {
			'destination': {
				'0': {
					'get:': '',
					'set:': '',
					'help:': '\'Five destinations of Trap sinks\'',
					'type:': 'UTF8String',
					'len:': '100'
				},
				'1': {
					'get:': '',
					'set:': '',
					'help:': '\'Five destinations of Trap sinks\'',
					'type:': 'UTF8String',
					'len:': '100'
				},
				'2': {
					'get:': '',
					'set:': '',
					'help:': '\'Five destinations of Trap sinks\'',
					'type:': 'UTF8String',
					'len:': '100'
				},
				'3': {
					'get:': '',
					'set:': '',
					'help:': '\'Five destinations of Trap sinks\'',
					'type:': 'UTF8String',
					'len:': '100'
				},
				'4': {
					'get:': '',
					'set:': '',
					'help:': '\'Five destinations of Trap sinks\'',
					'type:': 'UTF8String',
					'len:': '100'
				}
			},
			'trapCommunity': {
				'get:': '',
				'set:': '',
				'help:': '\'the default community is public\'',
				'len:': '30'
			}
		},
		'syslog': {
			'facility': {
				'get:': '',
				'set:': '',
				'help:': '\'LOG_LOCAL0 to LOG_LOCAL5\'',
				'len:': '63'
			}
		},
		'email': {
			'emailto': {
				'get:': '',
				'set:': '',
				'help:': '\'Destination for email messages\'',
				'len:': '100'
			},
			'smtpserver_ip_add': {
				'get:': '',
				'set:': '',
				'help:': '\'SMTP servers address\'',
				'type:': 'UTF8String',
				'len:': '100'
			},
			'emailport': {
				'get:': '',
				'set:': '',
				'help:': '\'Port number for sending email\'',
				'type:': 'int',
				'len:': '5'
			}
		},
		'sms': {
			'smsto': {
				'get:': '',
				'set:': '',
				'help:': '\'Destination address to send SMS\'',
				'len:': '40'
			},
			'sms_gateway_ip': {
				'get:': '',
				'set:': '',
				'help:': '\'SMS Gateway address\'',
				'type:': 'UTF8String',
				'len:': '100'
			},
			'smsport': {
				'get:': '',
				'set:': '',
				'help:': '\'Port number for sending sms\'',
				'type:': 'int',
				'len:': '5'
			}
		},
		'event': {
			'list:': '',
			'*': {
				'get:': '',
				'set:': '',
				'help:': '\'Event values in the range 0 to 32\'',
				'type:': 'int',
				'len:': '10',
				'mset': {
					'set:': ''
				}
			}
		}
	},
	'ipsec': {
		'nat_traversal': {
			'get:': '',
			'set:': '',
			'help:': '\'whether to accept/offer to support NAT workaround for IPsec.\''
		},
		'keep_alive': {
			'get:': '',
			'set:': '',
			'help:': '\'NAT keepalive time\'',
			'type:': 'int',
			'len:': '10'
		},
		'conn': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'help:': '\'the ipsec connection name\'',
			'len:': '50',
			'*': {
				'left': {
					'get:': '',
					'set:': '',
					'help:': '"the left connection participant, either an IP Address or %%defaultroute."'
				},
				'leftid': {
					'get:': '',
					'set:': '',
					'help:': '"identification for the left connection participant, for authentication purposes, as an IP address or @domainname"'
				},
				'leftrsasigkey': {
					'get:': '',
					'set:': '',
					'help:': '"the RSA signing key, in ipsec_datatot(3) format, or %none, %dnsondemand, %dnsonload, %dns or %cert."'
				},
				'leftsubnet': {
					'get:': '',
					'set:': '',
					'help:': '"the subnet behind the left connection participant, or blank for left/32."'
				},
				'leftnexthop': {
					'get:': '',
					'set:': '',
					'help:': '"an optional next-hop address for the left connection participant."'
				},
				'right': {
					'get:': '',
					'set:': '',
					'help:': '"the right connection participant, either an IP Address or %defaultroute."'
				},
				'rightid': {
					'get:': '',
					'set:': '',
					'help:': '"identification for the right connection participant, for authentication purposes, as an IP address or @domainname"'
				},
				'rightrsasigkey': {
					'get:': '',
					'set:': '',
					'help:': '"the RSA signing key, in ipsec_datatot(3) format, or %none, %dnsondemand, %dnsonload, %dns or %cert."'
				},
				'rightsubnet': {
					'get:': '',
					'set:': '',
					'help:': '"the subnet behind the right connection participant, or blank for right/32."'
				},
				'rightnexthop': {
					'get:': '',
					'set:': '',
					'help:': '"an optional next-hop address for the right connection participant."'
				},
				'auth': {
					'get:': '',
					'set:': '',
					'help:': '"the authentication protocol to use, \'ah\' or \'esp\'."'
				},
				'authby': {
					'get:': '',
					'set:': '',
					'help:': '"the type of authentication to use, \'secret\' for preshared secrets or \'rsasig\' for RSA keys."'
				},
				'auto': {
					'get:': '',
					'set:': '',
					'help:': '"the action to take at boot-time: \'add\', \'route\', \'start\' or \'ignore\'."'
				},
				'key': {
					'get:': '',
					'set:': '',
					'help:': '\'the secret key\''
				}
			}
		}
	},
	'iptables': {
		'list:': '',
		'*': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'help:': '\'list of chains\'',
			'len:': '31',
			'*': {
				'policy': {
					'get:': '',
					'set:': '',
					'help:': '\'the default policy of this chain\''
				},
				'targets': {
					'get:': '',
					'help:': '\'allowed targets for this chain\''
				},
				'rules': {
					'add:': '',
					'del:': '',
					'move:': '',
					'list:': '',
					'help:': '"list of rules for chain $chain, indexed by number, starting with 0."',
					'type:': 'int',
					'len:': '10',
					'*': {
						'destination': {
							'get:': '',
							'set:': '',
							'help:': '\'the source ip or network in dotted quad or cidr notation\''
						},
						'source': {
							'get:': '',
							'set:': '',
							'help:': '\'the source ip or network in dotted quad or cidr notation\''
						},
						'destination-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of source (yes or no)\'',
							'type:': 'boolean'
						},
						'source-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of source (yes or no)\'',
							'type:': 'boolean'
						},
						'protocol': {
							'get:': '',
							'set:': '',
							'help:': '\'the protocol: tcp, udp, icmp/icmpv6, all or a number\''
						},
						'protocol-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of protocol (yes or no)\'',
							'type:': 'boolean'
						},
						'dport': {
							'get:': '',
							'set:': '',
							'help:': '\'the port number (needs protocol tcp or udp)\'',
							'type:': 'tcpPort'
						},
						'sport': {
							'get:': '',
							'set:': '',
							'help:': '\'the port number (needs protocol tcp or udp)\'',
							'type:': 'tcpPort'
						},
						'dport-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion dport/sport (yes or no)"',
							'type:': 'boolean'
						},
						'sport-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion dport/sport (yes or no)"',
							'type:': 'boolean'
						},
						'tcp-flags': {
							'get:': '',
							'set:': '',
							'help:': '\'tcp flags\''
						},
						'tcp-flags-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of tcp-flags (yes or no)\'',
							'type:': 'boolean'
						},
						'icmp-type': {
							'get:': '',
							'set:': ''
						},
						'icmp-type-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of icmp-type (yes or no)\'',
							'type:': 'boolean'
						},
						'in-interface': {
							'get:': '',
							'set:': '',
							'help:': '\'the incoming interface, or empty for all\''
						},
						'in-interface-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion of $1 (yes or no)"',
							'type:': 'boolean'
						},
						'out-interface': {
							'get:': '',
							'set:': '',
							'help:': '\'the outgoing interface, or empty for all\''
						},
						'out-interface-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion of $1 (yes or no)"',
							'type:': 'boolean'
						},
						'fragment': {
							'get:': '',
							'set:': '',
							'help:': '\'"head", "nonhead" or "all" fragments\''
						},
						'target': {
							'get:': '',
							'set:': ''
						},
						'to-source': {
							'get:': '',
							'set:': '',
							'help:': '\'ip to nat to (single IP or IP:port)\''
						},
						'to-destination': {
							'get:': '',
							'set:': '',
							'help:': '\'ip to nat to (single IP or IP:port)\''
						},
						'reject-with': {
							'get:': '',
							'set:': '',
							'help:': '\'reject error\''
						},
						'log-level': {
							'get:': '',
							'set:': '',
							'help:': '\'the log level\''
						},
						'log-prefix': {
							'get:': '',
							'set:': '',
							'help:': '\'the log prefix\'',
							'len:': '29'
						}
					}
				}
			}
		}
	},
	'ip6tables': {
		'list:': '',
		'*': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'help:': '\'list of chains\'',
			'len:': '31',
			'*': {
				'policy': {
					'get:': '',
					'set:': '',
					'help:': '\'the default policy of this chain\''
				},
				'targets': {
					'get:': '',
					'help:': '\'allowed targets for this chain\''
				},
				'rules': {
					'add:': '',
					'del:': '',
					'move:': '',
					'list:': '',
					'help:': '"list of rules for chain $chain, indexed by number, starting with 0."',
					'type:': 'int',
					'len:': '10',
					'*': {
						'destination': {
							'get:': '',
							'set:': '',
							'help:': '\'the source ip or network in dotted quad or cidr notation\''
						},
						'source': {
							'get:': '',
							'set:': '',
							'help:': '\'the source ip or network in dotted quad or cidr notation\''
						},
						'destination-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of source (yes or no)\'',
							'type:': 'boolean'
						},
						'source-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of source (yes or no)\'',
							'type:': 'boolean'
						},
						'protocol': {
							'get:': '',
							'set:': '',
							'help:': '\'the protocol: tcp, udp, icmp/icmpv6, all or a number\''
						},
						'protocol-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of protocol (yes or no)\'',
							'type:': 'boolean'
						},
						'dport': {
							'get:': '',
							'set:': '',
							'help:': '\'the port number (needs protocol tcp or udp)\'',
							'type:': 'tcpPort'
						},
						'sport': {
							'get:': '',
							'set:': '',
							'help:': '\'the port number (needs protocol tcp or udp)\'',
							'type:': 'tcpPort'
						},
						'dport-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion dport/sport (yes or no)"',
							'type:': 'boolean'
						},
						'sport-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion dport/sport (yes or no)"',
							'type:': 'boolean'
						},
						'tcp-flags': {
							'get:': '',
							'set:': '',
							'help:': '\'tcp flags\''
						},
						'tcp-flags-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of tcp-flags (yes or no)\'',
							'type:': 'boolean'
						},
						'icmp-type': {
							'get:': '',
							'set:': ''
						},
						'icmp-type-inv': {
							'get:': '',
							'set:': '',
							'help:': '\'inversion of icmp-type (yes or no)\'',
							'type:': 'boolean'
						},
						'in-interface': {
							'get:': '',
							'set:': '',
							'help:': '\'the incoming interface, or empty for all\''
						},
						'in-interface-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion of $1 (yes or no)"',
							'type:': 'boolean'
						},
						'out-interface': {
							'get:': '',
							'set:': '',
							'help:': '\'the outgoing interface, or empty for all\''
						},
						'out-interface-inv': {
							'get:': '',
							'set:': '',
							'help:': '"inversion of $1 (yes or no)"',
							'type:': 'boolean'
						},
						'fragment': {
							'get:': '',
							'set:': '',
							'help:': '\'"head", "nonhead" or "all" fragments\''
						},
						'target': {
							'get:': '',
							'set:': ''
						},
						'to-source': {
							'get:': '',
							'set:': '',
							'help:': '\'ip to nat to (single IP or IP:port)\''
						},
						'to-destination': {
							'get:': '',
							'set:': '',
							'help:': '\'ip to nat to (single IP or IP:port)\''
						},
						'reject-with': {
							'get:': '',
							'set:': '',
							'help:': '\'reject error\''
						},
						'log-level': {
							'get:': '',
							'set:': '',
							'help:': '\'the log level\''
						},
						'log-prefix': {
							'get:': '',
							'set:': '',
							'help:': '\'the log prefix\'',
							'len:': '29'
						}
					}
				}
			}
		}
	},
	'logpipe': {
		'alert_low': {
			'get:': '',
			'set:': '',
			'help:': '\'alarm buffer threshold in bytes\'',
			'type:': 'int',
			'len:': '10'
		},
		'alert_med': {
			'get:': '',
			'set:': '',
			'help:': '\'alarm buffer threshold in bytes\'',
			'type:': 'int',
			'len:': '10'
		},
		'alert_high': {
			'get:': '',
			'set:': '',
			'help:': '\'alarm buffer threshold in bytes\'',
			'type:': 'int',
			'len:': '10'
		},
		'syslogport': {
			'get:': '',
			'set:': '',
			'help:': '\'the port where syslog listens on the remote server\'',
			'type:': 'int',
			'len:': '100'
		},
		'sshport': {
			'get:': '',
			'set:': '',
			'help:': '\'the remote ssh port\'',
			'type:': 'int',
			'len:': '5'
		},
		'server': {
			'get:': '',
			'set:': '',
			'help:': '\'the remote server where event data will be sent to\'',
			'len:': '100'
		},
		'user': {
			'get:': '',
			'set:': '',
			'help:': '\'the user on the remote server where event data will be sent to\'',
			'len:': '30'
		},
		'timeout': {
			'get:': '',
			'set:': '',
			'help:': '\'the idle timeout, after which the tunnel will be closed\'',
			'type:': 'int',
			'len:': '5'
		},
		'threshold': {
			'get:': '',
			'set:': '',
			'help:': '\'the threshold output buffer size when the tunnel will be started\'',
			'type:': 'int',
			'len:': '10'
		},
		'max_buf': {
			'get:': '',
			'set:': '',
			'help:': '\'the maximimum output buffer size\'',
			'type:': 'int',
			'len:': '10'
		},
		'enrolled-ip': {
			'get:': '',
			'set:': '',
			'type:': 'ipAddress'
		}
	},
	'ntpclient': {
		'server': {
			'get:': '',
			'set:': '',
			'help:': '\'the ntp server\'',
			'len:': '100'
		},
		'interval': {
			'get:': '',
			'set:': '',
			'help:': '\'the interval in seconds\'',
			'type:': 'int',
			'len:': '10'
		}
	},
	'service-type': {
		'list:': '',
		'type:': 'int',
		'len:': '10',
		'*': {
			'get:': '',
			'set:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		}
	},
	'snmpd': {
		'community': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'len:': '255',
			'*': {
				'permission': {
					'get:': '',
					'set:': '',
					'help:': '\'Permission, either read_only or read_write.\''
				},
				'source': {
					'get:': '',
					'set:': '',
					'help:': '\'the source network (CIDR notation, for example 10.0.0.0/24)\''
				},
				'oid': {
					'get:': '',
					'set:': '',
					'help:': '\'the OID\'',
					'len:': '40'
				}
			}
		},
		'community6': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'len:': '255',
			'*': {
				'permission': {
					'get:': '',
					'set:': '',
					'help:': '\'Permission, either read_only or read_write.\''
				},
				'source': {
					'get:': '',
					'set:': '',
					'help:': '\'the source network (CIDR notation, for example 10.0.0.0/24)\''
				},
				'oid': {
					'get:': '',
					'set:': '',
					'help:': '\'the OID\'',
					'len:': '40'
				}
			}
		},
		'user': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'len:': '255',
			'*': {
				'authmethod': {
					'get:': '',
					'set:': '',
					'help:': '\'authentication method. MD5 or SHA\''
				},
				'authpass': {
					'get:': '',
					'set:': '',
					'help:': '\'authentication passphrase, must be more than 8 characters\'',
					'len:': '50'
				},
				'cryptmethod': {
					'get:': '',
					'set:': '',
					'help:': '\'encryption method. Must be DES or AES.\''
				},
				'cryptpass': {
					'get:': '',
					'set:': '',
					'help:': '\'authentication passphrase, must be more than 8 characters\'',
					'len:': '50'
				},
				'permission': {
					'get:': '',
					'set:': '',
					'help:': '\'Permission, either read_only or read_write.\''
				},
				'level': {
					'get:': '',
					'set:': '',
					'help:': '\'the security level, one of noauth, auth or priv\''
				},
				'oid': {
					'get:': '',
					'set:': '',
					'help:': '\'the OID\'',
					'len:': '40'
				}
			}
		},
		'group': {
			'add:': '',
			'del:': '',
			'list:': '',
			'len:': '255',
			'*': {
				'sec_model': {
					'get:': '',
					'set:': ''
				},
				'sec_name': {
					'get:': '',
					'set:': '',
					'len:': '63'
				}
			}
		},
		'view': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'len:': '63',
			'*': {
				'list:': '',
				'*': {
					'incl_excl': {
						'get:': '',
						'set:': '',
						'help:': 'option(index($0, \'incl_excl\', \'subtree\', \'mask\'), \'included or excluded\', \'oid for the sub tree\', \'mask in hexadecimal\')'
					},
					'subtree': {
						'get:': '',
						'set:': '',
						'help:': 'option(index($0, \'incl_excl\', \'subtree\', \'mask\'), \'included or excluded\', \'oid for the sub tree\', \'mask in hexadecimal\')'
					},
					'mask': {
						'get:': '',
						'set:': '',
						'help:': 'option(index($0, \'incl_excl\', \'subtree\', \'mask\'), \'included or excluded\', \'oid for the sub tree\', \'mask in hexadecimal\')'
					}
				}
			}
		},
		'com2sec': {
			'add:': '',
			'del:': '',
			'list:': '',
			'*': {
				'context': {
					'get:': '',
					'set:': ''
				},
				'source': {
					'get:': '',
					'set:': '',
					'len:': '100'
				},
				'name': {
					'get:': '',
					'set:': ''
				}
			}
		},
		'access': {
			'add:': '',
			'del:': '',
			'list:': '',
			'*': {
				'add:': '',
				'del:': '',
				'move:': '',
				'list:': '',
				'*': {
					'noauth': {
						'match': {
							'get:': '',
							'set:': ''
						},
						'read': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						},
						'write': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						},
						'notif': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						}
					},
					'auth': {
						'match': {
							'get:': '',
							'set:': ''
						},
						'read': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						},
						'write': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						},
						'notif': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						}
					},
					'authpriv': {
						'match': {
							'get:': '',
							'set:': ''
						},
						'read': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						},
						'write': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						},
						'notif': {
							'get:': '',
							'set:': '',
							'help:': '\'a view or none\''
						}
					}
				}
			}
		},
		'proxy': {
			'add:': '',
			'del:': '',
			'move:': '',
			'list:': '',
			'*': {
				'context': {
					'get:': '',
					'set:': ''
				},
				'version': {
					'get:': '',
					'set:': ''
				},
				'community': {
					'get:': '',
					'set:': ''
				},
				'user': {
					'get:': '',
					'set:': ''
				},
				'oid': {
					'get:': '',
					'set:': '',
					'len:': '40'
				},
				'sec_level': {
					'get:': '',
					'set:': ''
				},
				'authmethod': {
					'get:': '',
					'set:': ''
				},
				'authpass': {
					'get:': '',
					'set:': ''
				},
				'privmethod': {
					'get:': '',
					'set:': ''
				},
				'privpass': {
					'get:': '',
					'set:': ''
				}
			}
		},
		'trap-forward': {
			'add:': '',
			'del:': '',
			'list:': '',
			'*': {
				'source': {
					'get:': '',
					'set:': '',
					'len:': '100'
				},
				'oid': {
					'get:': '',
					'set:': '',
					'len:': '40'
				},
				'destination': {
					'get:': '',
					'set:': ''
				}
			}
		},
		'syslocation': {
			'get:': '',
			'set:': '',
			'len:': '50'
		},
		'syscontact': {
			'get:': '',
			'set:': '',
			'len:': '50'
		}
	},
	'sshd': {
		'protocol': {
			'get:': '',
			'set:': '',
			'help:': '\'ssh protocol version. comma separated list of 1 and 2\''
		},
		'rootlogin': {
			'get:': '',
			'set:': '',
			'help:': '\'enable or disable login by root (yes or no)\'',
			'type:': 'boolean'
		},
		'port': {
			'get:': '',
			'set:': '',
			'help:': '\'the ssh port number\'',
			'type:': 'tcpPort'
		},
		'known-hosts': {
			'ssh-rsa': {
				'add:': '',
				'del:': '',
				'move:': '',
				'list:': '',
				'*': {
					'key': {
						'get:': '',
						'set:': '',
						'len:': '1024'
					}
				}
			}
		}
	},
	'syslog': {
		'destination': {
			'add:': '',
			'del:': '',
			'list:': '',
			'help:': '\'add a free defined destination name here\'',
			'*': {
				'enable': {
					'get:': '',
					'set:': '',
					'help:': '\'enable/disable destination (yes/no)\'',
					'type:': 'boolean'
				},
				'type': {
					'get:': '',
					'set:': '',
					'help:': '\'destination type, one of usertty, tcp, udp or file\''
				},
				'usertty': {
					'get:': '',
					'set:': '',
					'help:': '\'the owner of the tty (valid user name)\''
				},
				'tcp': {
					'get:': '',
					'set:': '',
					'help:': '\'the ip address or DNS name of a syslog server\'',
					'type:': 'ipv4Address',
					'len:': '15'
				},
				'udp': {
					'get:': '',
					'set:': '',
					'help:': '\'the ip address or DNS name of a syslog server\'',
					'type:': 'ipv4Address',
					'len:': '15'
				},
				'tcp6': {
					'get:': '',
					'set:': '',
					'help:': '\'the ip address or DNS name of a syslog server\'',
					'type:': 'ipv6Address',
					'len:': '39'
				},
				'udp6': {
					'get:': '',
					'set:': '',
					'help:': '\'the ip address or DNS name of a syslog server\'',
					'type:': 'ipv6Address',
					'len:': '39'
				},
				'file': {
					'get:': '',
					'set:': '',
					'help:': '\'a file name\''
				},
				'pipe': {
					'get:': '',
					'set:': '',
					'help:': '\'a pipe name\'',
					'template': {
						'get:': '',
						'set:': ''
					}
				}
			}
		},
		'filter': {
			'sysfilter': {
				'level': {
					'emerg': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					},
					'alert': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					},
					'crit': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					},
					'err': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					},
					'warn': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					},
					'notice': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					},
					'info': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					},
					'debug': {
						'get:': '',
						'set:': '',
						'help:': '"enable/disable logging of this level (yes or no)"'
					}
				}
			}
		},
		'presets': {
			'rootuser': {
				'get:': '',
				'set:': '',
				'help:': '\'set to yes or no for enabling syslog to the root user or console\'',
				'type:': 'boolean'
			},
			'console': {
				'get:': '',
				'set:': '',
				'help:': '\'set to yes or no for enabling syslog to the root user or console\'',
				'type:': 'boolean'
			},
			'remote-enable': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			},
			'remote6-enable': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			},
			'remote': {
				'add:': '',
				'del:': '',
				'list:': '',
				'help:': '\'add a remote destination\'',
				'len:': '64',
				'*': {
					'type': {
						'get:': '',
						'set:': '',
						'help:': '"destination type, one of $tcp or $udp"'
					}
				}
			},
			'remote6': {
				'add:': '',
				'del:': '',
				'list:': '',
				'help:': '\'add a remote destination\'',
				'len:': '64',
				'*': {
					'type': {
						'get:': '',
						'set:': '',
						'help:': '"destination type, one of $tcp or $udp"'
					}
				}
			}
		}
	},
	'userlevel': {
		'list:': '',
		'type:': 'int',
		'len:': '10',
		'*': {
			'get:': '',
			'set:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		}
	},
	'timezone': {
		'current': {
			'get:': '',
			'set:': '',
			'help:': '\'the time zone. Must be a filename relative to /usr/share/zoneinfo/, for example Etc/UTC.\''
		},
		'custom-file': {
			'add:': '',
			'list:': '',
			'*': {
				'rule': {
					'list:': '',
					'*': {
						'from': {
							'get:': '',
							'set:': ''
						},
						'to': {
							'get:': '',
							'set:': ''
						},
						'type': {
							'get:': '',
							'set:': ''
						},
						'in': {
							'get:': '',
							'set:': ''
						},
						'on': {
							'get:': '',
							'set:': ''
						},
						'at': {
							'get:': '',
							'set:': ''
						},
						'save': {
							'get:': '',
							'set:': ''
						},
						'letters': {
							'get:': '',
							'set:': ''
						}
					}
				},
				'zone': {
					'*': {
						'list:': '',
						'gmtoff': {
							'get:': '',
							'set:': ''
						},
						'rules': {
							'get:': '',
							'set:': ''
						},
						'format': {
							'get:': '',
							'set:': ''
						},
						'until': {
							'get:': '',
							'set:': ''
						}
					}
				},
				'link': {
					'*': {
						'list:': '',
						'link-from': {
							'get:': '',
							'set:': ''
						},
						'link-to': {
							'get:': '',
							'set:': ''
						}
					}
				},
				'leap': {
					'*': {
						'list:': '',
						'month': {
							'get:': '',
							'set:': ''
						},
						'day': {
							'get:': '',
							'set:': ''
						},
						'hhmmss': {
							'get:': '',
							'set:': ''
						},
						'corr': {
							'get:': '',
							'set:': ''
						},
						'rs': {
							'get:': '',
							'set:': ''
						}
					}
				}
			}
		},
		'custom': {
			'enable': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			},
			'name': {
				'get:': '',
				'set:': '',
				'type:': 'UTF8String',
				'len:': '4000'
			},
			'standard-acronym': {
				'get:': '',
				'set:': '',
				'type:': 'UTF8String',
				'len:': '4000'
			},
			'gmt-offset': {
				'get:': '',
				'set:': '',
				'type:': 'UTF8String',
				'len:': '4000'
			},
			'dst': {
				'dst-enable': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'dst-acronym': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'save-time': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'start-month': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'end-month': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'start-week': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'end-week': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'start-day': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'end-day': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'start-hour': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'end-hour': {
					'get:': '',
					'set:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				}
			}
		}
	},
	'userdefaults': {
		'passwd': {
			'max-days': {
				'get:': '',
				'set:': '',
				'help:': '\'the default maximum days until the password is changed\'',
				'type:': 'int',
				'len:': '5'
			},
			'min-days': {
				'get:': '',
				'set:': '',
				'help:': '\'the default minimum days until the password is changed\'',
				'type:': 'int',
				'len:': '5'
			},
			'warn-age': {
				'get:': '',
				'set:': '',
				'help:': '\'the default maximum days before password expires to warn user to change it.\'',
				'type:': 'int',
				'len:': '5'
			},
			'rules': {
				'enabled': {
					'get:': '',
					'set:': '',
					'type:': 'boolean'
				},
				'minlen': {
					'get:': '',
					'set:': '',
					'help:': '\'the minimum number of characters required in the password\'',
					'type:': 'int',
					'len:': '5'
				},
				'lowercase-required': {
					'get:': '',
					'set:': '',
					'help:': '\'minimum number of lower case characters/upper case characters/digits/special characters required in password\'',
					'type:': 'int',
					'len:': '5'
				},
				'uppercase-required': {
					'get:': '',
					'set:': '',
					'help:': '\'minimum number of lower case characters/upper case characters/digits/special characters required in password\'',
					'type:': 'int',
					'len:': '5'
				},
				'digit-required': {
					'get:': '',
					'set:': '',
					'help:': '\'minimum number of lower case characters/upper case characters/digits/special characters required in password\'',
					'type:': 'int',
					'len:': '5'
				},
				'special-required': {
					'get:': '',
					'set:': '',
					'help:': '\'minimum number of lower case characters/upper case characters/digits/special characters required in password\'',
					'type:': 'int',
					'len:': '5'
				}
			}
		},
		'account': {
			'expire-date': {
				'get:': '',
				'set:': '',
				'help:': '\'expire date in YYYY-MM-DD format\''
			},
			'inactive': {
				'get:': '',
				'set:': '',
				'help:': '\'The number of days after a password expires until the account is permanently disabled\''
			}
		}
	},
	'user': {
		'add:': '',
		'del:': '',
		'list:': '',
		'help:': '\'list of local users\'',
		'len:': '30',
		'userslistCLI': {
			'list:': '',
			'*': {
			}
		},
		'*': {
			'passwd': {
				'get:': '',
				'set:': '',
				'help:': '\'the password (write-only)\'',
				'len:': '127'
			},
			'hashpwd': {
				'get:': '',
				'set:': ''
			},
			'force-change': {
				'get:': '',
				'set:': '',
				'help:': '\'if set to yes, the user needs to change the password on the next login\'',
				'type:': 'boolean'
			},
			'uid': {
				'get:': '',
				'set:': '',
				'help:': '\'the user id (number, read-only)\'',
				'type:': 'int',
				'len:': '10'
			},
			'gid': {
				'get:': '',
				'set:': '',
				'help:': '\'the group id (number)\'',
				'type:': 'int',
				'len:': '10'
			},
			'group': {
				'get:': '',
				'set:': '',
				'help:': '\'the group (name)\'',
				'len:': '30'
			},
			'auth-group': {
				'add:': '',
				'del:': '',
				'list:': '',
				'*': {
				}
			},
			'gecos': {
				'get:': '',
				'set:': '',
				'help:': '\'the full name of the user\'',
				'len:': '63'
			},
			'shell': {
				'get:': '',
				'set:': '',
				'help:': '\'the shell of the user\'',
				'len:': '31'
			},
			'min': {
				'get:': '',
				'set:': '',
				'help:': '\'min/max/warn/inactive days for password change\'',
				'type:': 'int',
				'len:': '5'
			},
			'max': {
				'get:': '',
				'set:': '',
				'help:': '\'min/max/warn/inactive days for password change\'',
				'type:': 'int',
				'len:': '5'
			},
			'warn': {
				'get:': '',
				'set:': '',
				'help:': '\'min/max/warn/inactive days for password change\'',
				'type:': 'int',
				'len:': '5'
			},
			'inact': {
				'get:': '',
				'set:': '',
				'help:': '\'min/max/warn/inactive days for password change\'',
				'type:': 'int',
				'len:': '5'
			},
			'expire': {
				'get:': '',
				'set:': '',
				'help:': '\'expiration date in days since 1970-01-01 or empty for no expiration date\''
			},
			'expire-date': {
				'get:': '',
				'set:': '',
				'help:': '\'expiration date in YYYY-MM-DD format greater than present date, or empty for no expiration date\'',
				'len:': '10'
			}
		}
	},
	'group': {
		'add:': '',
		'del:': '',
		'list:': '',
		'help:': '\'list of local groups\'',
		'len:': '31',
		'*': {
			'gid': {
				'get:': '',
				'set:': '',
				'help:': '\'the group id (read only)\'',
				'type:': 'int',
				'len:': '10'
			},
			'members': {
				'get:': '',
				'set:': '',
				'list:': '',
				'help:': '\'comma separated list of members\''
			}
		}
	},
	'userprofile': {
		'type:': 'UTF8String',
		'len:': '4000',
		'*': {
			'profile': {
				'get:': ''
			},
			'cmd': {
				'get:': '',
				'set:': '',
				'type:': 'UTF8String',
				'len:': '100'
			},
			'options': {
				'get:': ''
			},
			'exit': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			}
		}
	},
	'power': {
		'global': {
			'cyclades': {
				'username': {
					'get:': '',
					'set:': '',
					'help:': '"$0 for $1 type PDUs"',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'password': {
					'get:': '',
					'set:': '',
					'help:': '"$0 for $1 type PDUs"',
					'type:': 'UTF8String',
					'len:': '4000'
				}
			},
			'spc': {
				'username': {
					'get:': '',
					'set:': '',
					'help:': '"$0 for $1 type PDUs"',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'password': {
					'get:': '',
					'set:': '',
					'help:': '"$0 for $1 type PDUs"',
					'type:': 'UTF8String',
					'len:': '4000'
				}
			},
			'servertech': {
				'username': {
					'get:': '',
					'set:': '',
					'help:': '"$0 for $1 type PDUs"',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'password': {
					'get:': '',
					'set:': '',
					'help:': '"$0 for $1 type PDUs"',
					'type:': 'UTF8String',
					'len:': '4000'
				}
			}
		},
		'chain': {
			'add:': '',
			'del:': '',
			'list:': '',
			'help:': '\'CHAIN ID\'',
			'type:': 'UTF8String',
			'len:': '32',
			'*': {
				'name': {
					'get:': '',
					'set:': '',
					'help:': '\'name of the chain\'',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'polling-rate': {
					'get:': '',
					'set:': '',
					'help:': '\'interval at which PDUs in the chain are polled in seconds\'',
					'type:': 'int',
					'len:': '10'
				},
				'pdu-type': {
					'get:': '',
					'set:': '',
					'help:': '\'PDU Vendor\'',
					'type:': 'UTF8String',
					'len:': '4000'
				}
			}
		},
		'pdu': {
			'add:': '',
			'del:': '',
			'list:': '',
			'help:': '\'PDU ID\'',
			'type:': 'UTF8String',
			'len:': '32',
			'pdu-local': {
				'id': {
					'get:': '',
					'set:': ''
				},
				'coldstart-delay': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'display-cycle': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'cycle-interval': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'voltage': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'current-lowcrit': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'current-low': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'current-warning': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'current-alarm': {
					'get:': '',
					'set:': '',
					'help:': '\'Delay before outlets are powered on after PDU booted\'',
					'type:': 'int',
					'len:': '10'
				},
				'hwocp': {
					'get:': '',
					'set:': '',
					'help:': '\'$0 on=1 or off=0\'',
					'type:': 'int',
					'len:': '10'
				},
				'overcurrent': {
					'get:': '',
					'set:': '',
					'help:': '\'$0 on=1 or off=0\'',
					'type:': 'int',
					'len:': '10'
				},
				'buzzer': {
					'get:': '',
					'set:': '',
					'help:': '\'$0 on=1 or off=0\'',
					'type:': 'int',
					'len:': '10'
				},
				'syslog': {
					'get:': '',
					'set:': '',
					'help:': '\'$0 on=1 or off=0\'',
					'type:': 'int',
					'len:': '10'
				},
				'outlet': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'outlet number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'outlet name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'outlet name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'ctrl-status': {
							'get:': '',
							'set:': '',
							'help:': '\'desired outlet status 0=off, 1=on\'',
							'type:': 'int',
							'len:': '10'
						},
						'mod-status': {
						},
						'post-on-delay': {
							'get:': '',
							'set:': '',
							'help:': '\'post-on and post-off delay for the outlet\'',
							'type:': 'int',
							'len:': '10'
						},
						'post-off-delay': {
							'get:': '',
							'set:': '',
							'help:': '\'post-on and post-off delay for the outlet\'',
							'type:': 'int',
							'len:': '10'
						},
						'wakeup': {
							'get:': '',
							'set:': '',
							'help:': '\'outlet state after coldstart, 0=off, 1=on, 2=last\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-lowcrit': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-low': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-warning': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-alarm': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				},
				'phase': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'phase number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'phase name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'phase name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-lowcrit': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-low': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-warning': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-alarm': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				},
				'circuit': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'circuit number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'circuit name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'circuit name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-lowcrit': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-low': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-warning': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-alarm': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				},
				'envmon': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'envmon number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'envmon name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'envmon name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'low-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'threshold\'',
							'type:': 'int',
							'len:': '10'
						},
						'low-warn-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'threshold\'',
							'type:': 'int',
							'len:': '10'
						},
						'high-warn-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'threshold\'',
							'type:': 'int',
							'len:': '10'
						},
						'high-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'threshold\'',
							'type:': 'int',
							'len:': '10'
						},
						'unit': {
							'get:': '',
							'set:': '',
							'help:': '\'threshold unit: 0 (oF,%,afu); 1 (oC); 2 (K)\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				}
			},
			'*': {
				'current-lowcrit': {
					'get:': '',
					'set:': '',
					'help:': '\'current threshold lowcrit/low/warning/alarm\'',
					'type:': 'int',
					'len:': '10'
				},
				'current-low': {
					'get:': '',
					'set:': '',
					'help:': '\'current threshold lowcrit/low/warning/alarm\'',
					'type:': 'int',
					'len:': '10'
				},
				'current-warning': {
					'get:': '',
					'set:': '',
					'help:': '\'current threshold lowcrit/low/warning/alarm\'',
					'type:': 'int',
					'len:': '10'
				},
				'current-alarm': {
					'get:': '',
					'set:': '',
					'help:': '\'current threshold lowcrit/low/warning/alarm\'',
					'type:': 'int',
					'len:': '10'
				},
				'num-phases': {
					'get:': '',
					'set:': '',
					'help:': '\'number of phases for ServerTech\'',
					'type:': 'int',
					'len:': '10'
				},
				'voltage': {
					'get:': '',
					'set:': '',
					'help:': '\'voltage\'',
					'type:': 'int',
					'len:': '10'
				},
				'power-factor': {
					'get:': '',
					'set:': '',
					'type:': 'int',
					'len:': '10'
				},
				'outlet': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'outlet number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'outlet name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'outlet name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-lowcrit': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-low': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-warning': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-alarm': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				},
				'phase': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'phase number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'phase name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'phase name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-lowcrit': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-low': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-warning': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-alarm': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				},
				'circuit': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'circuit number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'circuit name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'circuit name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-lowcrit': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-low': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-warning': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						},
						'current-alarm': {
							'get:': '',
							'set:': '',
							'help:': '\'current threshold lowcrit/low/warning/alarm\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				},
				'envmon': {
					'add:': '',
					'del:': '',
					'list:': '',
					'help:': '\'envmon number\'',
					'type:': 'int',
					'len:': '10',
					'*': {
						'name': {
							'get:': '',
							'set:': '',
							'help:': '\'envmon name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'sys-name': {
							'get:': '',
							'set:': '',
							'help:': '\'envmon name\'',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'low-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'temperature/humidity thresholds - low and high\'',
							'type:': 'int',
							'len:': '10'
						},
						'low-warn-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'temperature/humidity thresholds - low and high\'',
							'type:': 'int',
							'len:': '10'
						},
						'high-warn-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'temperature/humidity thresholds - low and high\'',
							'type:': 'int',
							'len:': '10'
						},
						'high-thres': {
							'get:': '',
							'set:': '',
							'help:': '\'temperature/humidity thresholds - low and high\'',
							'type:': 'int',
							'len:': '10'
						},
						'unit': {
							'get:': '',
							'set:': '',
							'help:': '\'threshold unit: 0 (oF,%,afu); 1 (oC); 2 (K)\'',
							'type:': 'int',
							'len:': '10'
						}
					}
				}
			}
		},
		'template': {
			'cyclades': {
				'pdu': {
					'max-current': {
						'get:': ''
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'voltage': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'power-factor': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'display-mode': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'phase': {
					'max-current': {
						'get:': ''
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'bank': {
					'max-current': {
						'get:': ''
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'outlet': {
					'post-power-on': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			},
			'avocent': {
				'pdu': {
					'max-current': {
						'get:': ''
					},
					'current-lowcrit': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-low': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-warning': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'cold-start-delay': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					}
				},
				'phase': {
					'max-current': {
						'get:': ''
					},
					'current-lowcrit': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-low': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-warning': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'bank': {
					'max-current': {
						'get:': ''
					},
					'current-lowcrit': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-low': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-warning': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'outlet': {
					'max-current': {
						'get:': ''
					},
					'current-lowcrit': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-low': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-warning': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'post-power-on': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'post-power-off': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			},
			'servertech': {
				'pdu': {
					'max-current': {
						'get:': ''
					},
					'current-low': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'voltage': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'cycle-interval': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'seq-interval': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'config-reset-button': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'cold-boot-alert': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'phase': {
					'max-current': {
						'get:': ''
					},
					'current-low': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'bank': {
					'max-current': {
						'get:': ''
					},
					'current-low': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'current-alarm': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'outlet': {
					'post-power-on': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'wakeup': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			},
			'spc': {
				'pdu': {
					'config-reset-button': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'cold-boot-alert': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'voltage': {
						'get:': '',
						'set:': '',
						'type:': 'int',
						'len:': '10'
					},
					'power-factor': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				},
				'phase': {
				},
				'bank': {
				},
				'outlet': {
					'wakeup': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'minimum-on': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'minimum-off': {
						'get:': '',
						'set:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					}
				}
			}
		}
	},
	'security': {
		'profile': {
			'get:': '',
			'set:': ''
		},
		'sshd': {
			'protocol': {
				'get:': '',
				'set:': ''
			},
			'rootlogin': {
				'get:': '',
				'set:': '',
				'type:': 'boolean'
			},
			'port': {
				'get:': '',
				'set:': '',
				'type:': 'tcpPort'
			}
		},
		'telnet': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'ftpd': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'portmap': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'ipsec': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'snmpd': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'icmp': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'http_enabled': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'https_enabled': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'http_redirect_to_https': {
			'get:': '',
			'set:': '',
			'type:': 'boolean'
		},
		'https_ssl_version': {
			'get:': '',
			'set:': '',
			'help:': '\'SSL protocol version for https. comma separated list of 2 and 3\''
		},
		'http_port': {
			'get:': '',
			'set:': '',
			'type:': 'tcpPort'
		},
		'https_port': {
			'get:': '',
			'set:': '',
			'type:': 'tcpPort'
		}
	}
}
