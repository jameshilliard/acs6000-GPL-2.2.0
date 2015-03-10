cn_tree = {
	'bdinfo': {
		'model_vm': {
			'info:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'model': {
			'info:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'product': {
			'info:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'mac2': {
			'info:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'fpgaver': {
			'info:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'ports': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'power1st': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'power2st': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'serial#': {
			'info:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'mac1': {
			'info:': '',
			'type:': 'UTF8String',
			'len:': '4000'
		},
		'family': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'power': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'modem': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'aux': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		}
	},
	'network': {
		'interface': {
			'list:': '',
			'*': {
				'address': {
					'info:': '',
					'help:': '\'IPv4 address\''
				},
				'address6': {
					'info:': '',
					'help:': '\'list of IPv6 addresses, separated by spaces\''
				},
				'netmask': {
					'info:': ''
				},
				'broadcast': {
					'info:': ''
				},
				'hwaddr': {
					'info:': ''
				},
				'ifconfig': {
					'info:': ''
				},
				'is-pluggable': {
					'info:': '',
					'help:': '\'"1" if this network interface is a pluggable device, "0" otherwise.\'',
					'type:': 'boolean'
				},
				'is-wireless': {
					'info:': '',
					'help:': '\'"1" if this network interface is a wireless device, "0" otherwise.\'',
					'type:': 'boolean'
				}
			}
		}
	},
	'system': {
		'version': {
			'info:': ''
		},
		'loadavg': {
			'info:': ''
		},
		'uptime': {
			'info:': '',
			'type:': 'double',
			'len:': '20'
		},
		'idle': {
			'info:': '',
			'type:': 'double',
			'len:': '20'
		},
		'cpuinfo': {
			'list:': '',
			'info:': '',
			'*': {
				'info:': ''
			}
		},
		'meminfo': {
			'list:': '',
			'info:': '',
			'*': {
				'info:': ''
			}
		},
		'df': {
			'info:': ''
		},
		'ps': {
			'info:': ''
		},
		'booted-from': {
			'info:': ''
		},
		'route-ipv4': {
			'info:': ''
		},
		'route-ipv6': {
			'info:': ''
		},
		'reboot': {
			'admin:': ''
		}
	},
	'session': {
		'list:': '',
		'type:': 'int',
		'len:': '10',
		'*': {
			'kill': {
				'admin:': ''
			},
			'username': {
				'info:': ''
			},
			'groupname': {
				'info:': ''
			},
			'creation_time': {
				'info:': ''
			},
			'creation_time_raw': {
				'info:': ''
			},
			'creation_time_ms': {
				'info:': ''
			},
			'duration_time': {
				'info:': ''
			},
			'duration_time_ms': {
				'info:': ''
			},
			'subsessions': {
				'info:': ''
			},
			'parent_ref': {
				'info:': ''
			},
			'flags': {
				'info:': ''
			},
			'session_type': {
				'info:': ''
			},
			'connection_type': {
				'info:': ''
			},
			'connection_id': {
				'info:': '',
				'type:': 'int',
				'len:': '10'
			},
			'idle_timeout': {
				'info:': ''
			},
			'mpid': {
				'info:': ''
			},
			'client_IP': {
				'info:': ''
			},
			'port': {
				'info:': ''
			},
			'test_create': {
				'admin:': ''
			},
			'test_createsub': {
				'admin:': ''
			},
			'test_delete': {
				'admin:': ''
			},
			'test_setconnid': {
				'admin:': ''
			},
			'test_sid': {
				'info:': ''
			}
		},
		'get_ref': {
			'info:': ''
		},
		'all': {
			'list:': ''
		}
	},
	'target': {
		'pdu': {
			'list:': '',
			'*': {
				'connect': {
					'admin:': ''
				}
			}
		},
		'outlet_group': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'status': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'on': {
					'admin:': ''
				},
				'off': {
					'admin:': ''
				},
				'cycle': {
					'admin:': ''
				},
				'lock': {
					'admin:': ''
				},
				'unlock': {
					'admin:': ''
				},
				'power': {
					'info:': ''
				}
			}
		}
	},
	'wmi': {
		'applet_viewer': {
			'username': {
				'info:': ''
			},
			'password': {
				'info:': ''
			}
		}
	},
	'appliance': {
		'saveconf-local': {
			'admin:': ''
		},
		'saveconf-ftp': {
			'admin:': ''
		},
		'saveconf-tftp': {
			'admin:': ''
		},
		'saveconf-ssh': {
			'admin:': ''
		},
		'restoreconf-factorydefault': {
			'admin:': ''
		},
		'restoreconf-local': {
			'admin:': ''
		},
		'restoreconf-ftp': {
			'admin:': ''
		},
		'restoreconf-tftp': {
			'admin:': ''
		},
		'restoreconf-ssh': {
			'admin:': ''
		},
		'removeconf-local': {
			'admin:': ''
		},
		'shell': {
			'info:': ''
		},
		'scp': {
			'info:': ''
		},
		'ftp': {
			'info:': ''
		},
		'inspect_dla': {
			'info:': ''
		},
		'clean_dla': {
			'info:': ''
		}
	},
	'connection': {
		'outlet': {
			'list:': '',
			'*': {
				'status': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'on': {
					'admin:': ''
				},
				'off': {
					'admin:': ''
				},
				'cycle': {
					'admin:': ''
				}
			}
		},
		'serial': {
			'*': {
				'actions': {
					'info:': ''
				},
				'serial_connect': {
					'info:': ''
				},
				'serial_share': {
					'info:': ''
				},
				'serial_sniff': {
					'info:': ''
				},
				'serial_close': {
					'info:': ''
				},
				'serial_sendmsg': {
					'list:': '',
					'*': {
						'info:': ''
					}
				},
				'serial_killuser': {
					'list:': '',
					'*': {
						'info:': ''
					}
				},
				'serial_listuser': {
					'list:': ''
				},
				'serial_power': {
					'is_power': {
						'info:': ''
					},
					'status': {
						'info:': ''
					},
					'on': {
						'admin:': ''
					},
					'off': {
						'admin:': ''
					},
					'cycle': {
						'admin:': ''
					},
					'lock': {
						'admin:': ''
					},
					'unlock': {
						'admin:': ''
					}
				},
				'serial_inspect_dbuf': {
					'info:': ''
				},
				'serial_clean_dbuf': {
					'info:': ''
				}
			}
		}
	},
	'date': {
		'admin:': '',
		'info:': '',
		'help:': '\'get the date (info) or set the date (admin)\'',
		'verify-current-date-with-certificate': {
			'info:': ''
		},
		'verify-date-with-certificate': {
			'info:': ''
		},
		'YMDHMS': {
			'info:': ''
		},
		'MDY': {
			'admin:': '',
			'info:': ''
		},
		'HHMMSS': {
			'admin:': '',
			'info:': ''
		},
		'seconds-since-epoch': {
			'admin:': '',
			'info:': ''
		},
		'test-within-ssl-limits': {
		}
	},
	'dialin-secure': {
		'ppp-otp-user': {
			'adduser': {
				'admin:': ''
			},
			'deluser': {
				'admin:': ''
			},
			'userlist': {
				'list:': '',
				'*': {
					'id': {
						'info:': '',
						'help:': '\'Opie user id\''
					}
				}
			}
		}
	},
	'ipsec': {
		'rsakey': {
			'admin:': '',
			'info:': ''
		}
	},
	'iptables': {
		'filter': {
			'list:': '',
			'*': {
				'counters': {
					'info:': ''
				},
				'rules': {
					'list:': '',
					'*': {
						'counters': {
							'info:': ''
						}
					}
				}
			}
		},
		'nat': {
			'list:': '',
			'*': {
				'counters': {
					'info:': ''
				},
				'rules': {
					'list:': '',
					'*': {
						'counters': {
							'info:': ''
						}
					}
				}
			}
		}
	},
	'ip6tables': {
		'filter': {
			'list:': '',
			'*': {
				'counters': {
					'info:': ''
				},
				'rules': {
					'list:': '',
					'*': {
						'counters': {
							'info:': ''
						}
					}
				}
			}
		}
	},
	'pluggable': {
		'insert': {
			'admin:': '',
			'help:': '\'To insert a card\''
		},
		'existdev': {
			'info:': '',
			'help:': '\'"1" if the device exists and is pluggable, "0" otherwise\'',
			'type:': 'boolean'
		},
		'modemcheck': {
			'info:': ''
		},
		'devlist': {
			'list:': '',
			'*': {
				'slot': {
					'info:': '',
					'help:': '\'Slot in which the device is plugged\''
				},
				'type': {
					'info:': '',
					'help:': '\'Device Type\''
				},
				'card': {
					'info:': '',
					'help:': '\'Card type slot\''
				},
				'deviceName': {
					'info:': '',
					'help:': '\'Devices\''
				},
				'eject': {
					'admin:': '',
					'help:': '\'To eject a card\''
				},
				'del': {
					'admin:': '',
					'help:': '\'To del a card\''
				},
				'summary': {
					'info:': '',
					'help:': '\'Card info\''
				}
			}
		},
		'ejected_devlist': {
			'list:': '',
			'*': {
				'slot': {
					'info:': '',
					'help:': '\'Slot in which the device is plugged\''
				},
				'type': {
					'info:': '',
					'help:': '\'Device Type\''
				},
				'card': {
					'info:': '',
					'help:': '\'Card type slot\''
				},
				'deviceName': {
					'info:': '',
					'help:': '\'Devices\''
				},
				'del': {
					'admin:': '',
					'help:': '\'To del a card\''
				},
				'summary': {
					'info:': '',
					'help:': '\'Card info\''
				}
			}
		}
	},
	'port': {
		'serial': {
			'list:': '',
			'type:': 'int',
			'len:': '10',
			'*': {
				'summary': {
					'info:': ''
				},
				'status': {
					'signals': {
						'info:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'counters': {
						'info:': '',
						'type:': 'UTF8String',
						'len:': '4000'
					},
					'DTR': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'RTS': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'CTS': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'DCD': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'tx': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'rx': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'fe': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'pe': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'brk': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'oe': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'baud': {
						'info:': '',
						'type:': 'int',
						'len:': '10'
					},
					'devicename': {
						'info:': ''
					},
					'sessions': {
						'info:': ''
					}
				}
			},
			'portipadd': {
				'info:': ''
			},
			'porttcpalias': {
				'info:': ''
			},
			'portorpoolipadd': {
				'info:': ''
			},
			'aliasgetport': {
				'info:': ''
			},
			'aliasgetconid': {
				'info:': ''
			}
		}
	},
	'topology': {
		'casport': {
			'info:': '',
			'*': {
				'info:': ''
			}
		},
		'dirconinfo': {
			'info:': ''
		},
		'powerport': {
			'info:': ''
		},
		'tsmenuinfo': {
			'info:': ''
		}
	},
	'passwd': {
		'admin:': ''
	},
	'whoami': {
		'info:': ''
	},
	'power': {
		'port': {
			'list:': '',
			'type:': 'int',
			'len:': '10',
			'*': {
				'name': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'pdu-type': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'num-pdus': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'number': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'device': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'polling-rate': {
					'admin:': '',
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'overcurrent': {
					'list:': '',
					'admin:': '',
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'syslog': {
					'list:': '',
					'admin:': '',
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'buzzer': {
					'list:': '',
					'admin:': '',
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'cycle-interval': {
					'admin:': '',
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'upgrade': {
					'admin:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'save': {
					'admin:': ''
				},
				'pdu': {
					'list:': '',
					'type:': 'int',
					'len:': '10',
					'*': {
						'list:': '',
						'id': {
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'defaultid': {
							'info:': ''
						},
						'version': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'model': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'max-current': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'model_nr': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'model_ext': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'vendor': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'islocal': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'switched': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'fpga': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'num-inlets': {
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'inlet-type': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'overcurrent': {
							'list:': '',
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'syslog': {
							'list:': '',
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'buzzer': {
							'list:': '',
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'config-reset-button': {
							'list:': '',
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'cold-boot-alert': {
							'list:': '',
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'display-mode': {
							'list:': '',
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'display-cycle': {
							'admin:': '',
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'cycle-interval': {
							'admin:': '',
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'seq-interval': {
							'admin:': '',
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'hwocp': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000',
							'reset': {
								'admin:': ''
							}
						},
						'save': {
							'admin:': ''
						},
						'restore': {
							'admin:': ''
						},
						'reboot': {
							'admin:': ''
						},
						'factory-default': {
							'admin:': ''
						},
						'upgrade': {
							'admin:': '',
							'info:': ''
						},
						'voltage-min': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'voltage-max': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'voltage-avg': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'voltage-type': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'voltage': {
							'list:': '',
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-min': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-max': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-avg': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'threshold-status': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'consolidated-alarm': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-lowcrit': {
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-low': {
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-warning': {
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'current-alarm': {
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'topology': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-min': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-max': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-avg': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-type': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-factor-min': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-factor-max': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-factor-avg': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-factor-type': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'power-factor': {
							'admin:': '',
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'reset-voltage': {
							'admin:': ''
						},
						'reset-current': {
							'admin:': ''
						},
						'reset-power': {
							'admin:': ''
						},
						'reset-power-factor': {
							'admin:': ''
						},
						'cumulative-watthour': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000',
							'reset': {
								'admin:': ''
							}
						},
						'cumulative-starttime': {
							'info:': '',
							'type:': 'UTF8String',
							'len:': '4000'
						},
						'cold-start-delay': {
							'admin:': '',
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'on': {
							'admin:': ''
						},
						'off': {
							'admin:': ''
						},
						'cycle': {
							'admin:': ''
						},
						'ion': {
							'admin:': ''
						},
						'ioff': {
							'admin:': ''
						},
						'num-outlets-on': {
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'num-outlets': {
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'true-num-outlets': {
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'outlet': {
							'list:': '',
							'type:': 'int',
							'len:': '10',
							'*': {
								'list:': '',
								'name': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'sys-name': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'ctrl-status': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'mod-status': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'relay-status': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'minimum-on': {
									'list:': '',
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'minimum-off': {
									'list:': '',
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'post-power-on': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'post-power-off': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'wakeup': {
									'list:': '',
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'hwocp': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'threshold-status': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'voltage': {
									'info:': ''
								},
								'voltage-min': {
									'info:': ''
								},
								'voltage-max': {
									'info:': ''
								},
								'voltage-avg': {
									'info:': ''
								},
								'voltage-type': {
									'info:': ''
								},
								'power': {
									'info:': ''
								},
								'power-min': {
									'info:': ''
								},
								'power-max': {
									'info:': ''
								},
								'power-avg': {
									'info:': ''
								},
								'power-type': {
									'info:': ''
								},
								'power-factor': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-type': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'cumulative-watthour': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000',
									'reset': {
										'admin:': ''
									}
								},
								'cumulative-starttime': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'reset-voltage': {
									'admin:': ''
								},
								'reset-current': {
									'admin:': ''
								},
								'reset-power': {
									'admin:': ''
								},
								'reset-power-factor': {
									'admin:': ''
								},
								'current-low': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-lowcrit': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-warning': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-alarm': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'on': {
									'admin:': ''
								},
								'off': {
									'admin:': ''
								},
								'cycle': {
									'admin:': ''
								},
								'lock': {
									'admin:': ''
								},
								'unlock': {
									'admin:': ''
								},
								'ion': {
									'admin:': ''
								},
								'ioff': {
									'admin:': ''
								}
							}
						},
						'num-phases': {
							'admin:': '',
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'phase': {
							'list:': '',
							'type:': 'int',
							'len:': '10',
							'*': {
								'list:': '',
								'name': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'sys-name': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'threshold-status': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'hwocp': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'reset-current': {
									'admin:': ''
								},
								'current-low': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-lowcrit': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-warning': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-alarm': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-type': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-type': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'reset-power': {
									'admin:': ''
								},
								'reset-power-factor': {
									'admin:': ''
								},
								'cumulative-watthour': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000',
									'reset': {
										'admin:': ''
									}
								},
								'cumulative-starttime': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'voltage': {
									'info:': ''
								},
								'voltage-min': {
									'info:': ''
								},
								'voltage-max': {
									'info:': ''
								},
								'voltage-avg': {
									'info:': ''
								},
								'voltage-type': {
									'info:': ''
								},
								'reset-voltage': {
									'admin:': ''
								}
							}
						},
						'num-envmons': {
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'envmon': {
							'list:': '',
							'type:': 'int',
							'len:': '10',
							'*': {
								'list:': '',
								'name': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'sys-name': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'type': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'type-raw': {
									'info:': '',
									'type:': 'int',
									'len:': '10'
								},
								'unit': {
									'list:': '',
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'low-thres': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'low-warn-thres': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'high-warn-thres': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'high-thres': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'value': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'min-value': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'max-value': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'avg-value': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'num-samples': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'thres-status': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'value-units': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'min-value-units': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'max-value-units': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'avg-value-units': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'reset': {
									'admin:': ''
								}
							}
						},
						'num-circuits': {
							'info:': '',
							'type:': 'int',
							'len:': '10'
						},
						'circuit': {
							'list:': '',
							'type:': 'int',
							'len:': '10',
							'*': {
								'list:': '',
								'name': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'sys-name': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-type': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-factor-type': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'reset-power-factor': {
									'admin:': ''
								},
								'cumulative-watthour': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000',
									'reset': {
										'admin:': ''
									}
								},
								'cumulative-starttime': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'num-outlets': {
									'info:': '',
									'type:': 'int',
									'len:': '10'
								},
								'current': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'threshold-status': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'voltage': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'voltage-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'voltage-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'voltage-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'voltage-type': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-min': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-max': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'power-avg': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'reset-voltage': {
									'admin:': ''
								},
								'reset-current': {
									'admin:': ''
								},
								'reset-power': {
									'admin:': ''
								},
								'current-low': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-lowcrit': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-warning': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'current-alarm': {
									'admin:': '',
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								},
								'blown-fuse': {
									'info:': '',
									'type:': 'UTF8String',
									'len:': '4000'
								}
							}
						}
					}
				}
			}
		},
		'num-devices': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'device': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'port': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'num-pdus': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'pdu': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'session': {
				},
				'pos': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'vendor': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'port': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'pdu': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'num-outlets': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'outlet': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'pos': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'port': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'pdu': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'outlet': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'circuit': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'num-phases': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'phase': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'pos': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'port': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'pdu': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'phase': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'num-circuits': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'circuit': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'pos': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'port': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'pdu': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'circuit': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'num-envmons': {
			'info:': '',
			'type:': 'int',
			'len:': '10'
		},
		'envmon': {
			'list:': '',
			'type:': 'UTF8String',
			'len:': '4000',
			'*': {
				'pos': {
					'info:': '',
					'type:': 'UTF8String',
					'len:': '4000'
				},
				'port': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'pdu': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				},
				'envmon': {
					'info:': '',
					'type:': 'int',
					'len:': '10'
				}
			}
		},
		'command': {
			'on': {
				'admin:': ''
			},
			'off': {
				'admin:': ''
			},
			'cycle': {
				'admin:': ''
			},
			'lock': {
				'admin:': ''
			},
			'unlock': {
				'admin:': ''
			},
			'ion': {
				'admin:': ''
			},
			'ioff': {
				'admin:': ''
			}
		},
		'servertech': {
			'admin:': '',
			'pubkey': {
				'info:': ''
			},
			'detected': {
				'list:': '',
				'info:': ''
			},
			'licensed': {
				'list:': '',
				'info:': ''
			}
		},
		'firmware': {
			'load': {
				'admin:': '',
				'help:': '"<SERVER> <USERNAME> <PASSWORD> <FILE PATH>"'
			},
			'removefile': {
				'info:': ''
			},
			'save': {
				'admin:': '',
				'type:': 'UTF8String',
				'len:': '4000'
			},
			'version': {
				'info:': '',
				'type:': 'UTF8String',
				'len:': '4000'
			}
		},
		'wait-shm': {
			'admin:': ''
		},
		'backup': {
			'admin:': ''
		},
		'restore': {
			'admin:': ''
		},
		'passwd': {
			'admin:': ''
		},
		'apply-template': {
			'admin:': ''
		},
		'global-alarm-count': {
			'info:': ''
		}
	},
	'opie': {
		'opiepasswd': {
			'info:': ''
		}
	},
	'firmware': {
		'version': {
			'info:': ''
		},
		'bootversion': {
			'info:': ''
		},
		'pluginversion': {
			'info:': ''
		},
		'family': {
			'info:': ''
		},
		'partnumbers': {
			'info:': ''
		},
		'oems': {
			'info:': ''
		},
		'ccode': {
			'info:': ''
		},
		'lcode': {
			'info:': ''
		},
		'date': {
			'info:': ''
		},
		'bootcode-date': {
			'info:': ''
		},
		'upgrade': {
			'load': {
				'admin:': '',
				'help:': '\'<SERVER> <USERNAME> <PASSWORD> <DIRECTORY> <FILENAME>\''
			},
			'version': {
				'info:': ''
			},
			'date': {
				'info:': ''
			},
			'time': {
				'info:': ''
			},
			'family': {
				'info:': ''
			},
			'partnumbers': {
				'info:': ''
			},
			'oems': {
				'info:': ''
			},
			'error': {
				'info:': ''
			},
			'warning': {
				'info:': ''
			},
			'downgrade': {
				'info:': ''
			},
			'remove': {
				'admin:': ''
			},
			'save': {
				'admin:': ''
			},
			'saveboot': {
				'admin:': ''
			}
		}
	}
}
