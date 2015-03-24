#!/usr/bin/python
"""
===Introduction===
The following is hacked together from a subset of Cura files. The intended purpose is to convert .ini 
Cura profiles to .cfg CuraEngine profiles. 
"""
__copyright__ = "Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License"

from optparse import OptionParser

from util import profile

def main():
	parser = OptionParser(usage="usage: %prog [options]")
	
	parser.add_option("-i", "--ini", action="store", type="string", dest="profileini",
		help="Load settings from a profile ini file")
	parser.add_option("-o", "--output", action="store", type="string", dest="output",
		help="path to write machine config file to")


	(options, args) = parser.parse_args()
	if options.output is None:
		options.output = "machine.cfg"
		
	if options.profileini is not None:
			load_profile(options.profileini, options.output)



def load_profile(src, output):
	print "Load preferences from ", src
	profile.loadProfile(src)
	print profile.getProfileSettingFloat("print_speed");
	e_settings = _engineSettings(profile, 1)

	file_ = open(output, 'w')

	settings_dict = {}
	for k in e_settings:
		# print k, e_settings[k]
		settings_dict[k] = e_settings[k]
	
	for k in sorted(settings_dict):
		print k, settings_dict[k]
		file_.write(str(k) + " = " + str(settings_dict[k]) + "\n")
	file_.close()
	print "Successfully written to", output


def _engineSettings(profile, extruderCount):
	print profile.getProfileSettingFloat("bottom_thickness");
	print profile.getProfileSettingFloat("print_speed");
	settings = {
		'layerThickness': int(profile.getProfileSettingFloat('layer_height') * 1000),
		'initialLayerThickness': int(profile.getProfileSettingFloat('bottom_thickness') * 1000) if profile.getProfileSettingFloat('bottom_thickness') > 0.0 else int(profile.getProfileSettingFloat('layer_height') * 1000),
		'filamentDiameter': int(profile.getProfileSettingFloat('filament_diameter') * 1000),
		'filamentFlow': int(profile.getProfileSettingFloat('filament_flow')),
		'extrusionWidth': int(profile.calculateEdgeWidth() * 1000),
		'layer0extrusionWidth': int(profile.calculateEdgeWidth() * profile.getProfileSettingFloat('layer0_width_factor') / 100 * 1000),
		'insetCount': int(profile.calculateLineCount()),
		'downSkinCount': int(profile.calculateSolidLayerCount()) if profile.getProfileSetting('solid_bottom') == 'True' else 0,
		'upSkinCount': int(profile.calculateSolidLayerCount()) if profile.getProfileSetting('solid_top') == 'True' else 0,
		'infillOverlap': int(profile.getProfileSettingFloat('fill_overlap')),
		'perimeterBeforeInfill': 1 if profile.getProfileSetting('perimeter_before_infill') == 'True' else 0,
		'initialSpeedupLayers': int(4),
		'initialLayerSpeed': int(profile.getProfileSettingFloat('bottom_layer_speed')),
		'printSpeed': int(profile.getProfileSettingFloat('print_speed')),
		'infillSpeed': int(profile.getProfileSettingFloat('infill_speed')) if int(profile.getProfileSettingFloat('infill_speed')) > 0 else int(profile.getProfileSettingFloat('print_speed')),
		'inset0Speed': int(profile.getProfileSettingFloat('inset0_speed')) if int(profile.getProfileSettingFloat('inset0_speed')) > 0 else int(profile.getProfileSettingFloat('print_speed')),
		'insetXSpeed': int(profile.getProfileSettingFloat('insetx_speed')) if int(profile.getProfileSettingFloat('insetx_speed')) > 0 else int(profile.getProfileSettingFloat('print_speed')),
		'skinSpeed': int(profile.getProfileSettingFloat('solidarea_speed')) if int(profile.getProfileSettingFloat('solidarea_speed')) > 0 else int(profile.getProfileSettingFloat('print_speed')),
		'moveSpeed': int(profile.getProfileSettingFloat('travel_speed')),
		'fanSpeedMin': int(profile.getProfileSettingFloat('fan_speed')) if profile.getProfileSetting('fan_enabled') == 'True' else 0,
		'fanSpeedMax': int(profile.getProfileSettingFloat('fan_speed_max')) if profile.getProfileSetting('fan_enabled') == 'True' else 0,
		'supportAngle': int(-1) if profile.getProfileSetting('support') == 'None' else int(profile.getProfileSettingFloat('support_angle')),
		'supportEverywhere': int(1) if profile.getProfileSetting('support') == 'Everywhere' else int(0),
		'supportLineDistance': int(100 * profile.calculateEdgeWidth() * 1000 / profile.getProfileSettingFloat('support_fill_rate')) if profile.getProfileSettingFloat('support_fill_rate') > 0 else -1,
		'supportXYDistance': int(1000 * profile.getProfileSettingFloat('support_xy_distance')),
		'supportZDistance': int(1000 * profile.getProfileSettingFloat('support_z_distance')),
		'supportExtruder': 0 if profile.getProfileSetting('support_dual_extrusion') == 'First extruder' else (1 if profile.getProfileSetting('support_dual_extrusion') == 'Second extruder' and profile.minimalExtruderCount() > 1 else -1),
		'retractionAmount': int(profile.getProfileSettingFloat('retraction_amount') * 1000) if profile.getProfileSetting('retraction_enable') == 'True' else 0,
		'retractionSpeed': int(profile.getProfileSettingFloat('retraction_speed')),
		'retractionMinimalDistance': int(profile.getProfileSettingFloat('retraction_min_travel') * 1000),
		'retractionAmountExtruderSwitch': int(profile.getProfileSettingFloat('retraction_dual_amount') * 1000),
		'retractionZHop': int(profile.getProfileSettingFloat('retraction_hop') * 1000),
		'minimalExtrusionBeforeRetraction': int(profile.getProfileSettingFloat('retraction_minimal_extrusion') * 1000),
		'multiVolumeOverlap': int(profile.getProfileSettingFloat('overlap_dual') * 1000),
		'objectSink': max(0, int(profile.getProfileSettingFloat('object_sink') * 1000)),
		'minimalLayerTime': int(profile.getProfileSettingFloat('cool_min_layer_time')),
		'minimalFeedrate': int(profile.getProfileSettingFloat('cool_min_feedrate')),
		'coolHeadLift': 1 if profile.getProfileSetting('cool_head_lift') == 'True' else 0,
		# 'startCode': profile.getAlterationFileContents('start.gcode', extruderCount),
		# 'endCode': profile.getAlterationFileContents('end.gcode', extruderCount),
		# 'preSwitchExtruderCode': profile.getAlterationFileContents('preSwitchExtruder.gcode', extruderCount),
		# 'postSwitchExtruderCode': profile.getAlterationFileContents('postSwitchExtruder.gcode', extruderCount),

		'extruderOffset[1].X': int(profile.getMachineSettingFloat('extruder_offset_x1') * 1000),
		'extruderOffset[1].Y': int(profile.getMachineSettingFloat('extruder_offset_y1') * 1000),
		'extruderOffset[2].X': int(profile.getMachineSettingFloat('extruder_offset_x2') * 1000),
		'extruderOffset[2].Y': int(profile.getMachineSettingFloat('extruder_offset_y2') * 1000),
		'extruderOffset[3].X': int(profile.getMachineSettingFloat('extruder_offset_x3') * 1000),
		'extruderOffset[3].Y': int(profile.getMachineSettingFloat('extruder_offset_y3') * 1000),
		'fixHorrible': 0,
	}
	fanFullHeight = int(profile.getProfileSettingFloat('fan_full_height') * 1000)
	settings['fanFullOnLayerNr'] = (fanFullHeight - settings['initialLayerThickness'] - 1) / settings['layerThickness'] + 1
	if settings['fanFullOnLayerNr'] < 0:
		settings['fanFullOnLayerNr'] = 0
	if profile.getProfileSetting('retraction_combing') == 'All':
		settings['enableCombing'] = 1
	elif profile.getProfileSetting('retraction_combing') == 'No Skin':
		settings['enableCombing'] = 2
	else:
		settings['enableCombing'] = 0
	if profile.getProfileSetting('support_type') == 'Lines':
		settings['supportType'] = 1

	if profile.getProfileSettingFloat('fill_density') == 0:
		settings['sparseInfillLineDistance'] = -1
	elif profile.getProfileSettingFloat('fill_density') == 100:
		settings['sparseInfillLineDistance'] = settings['extrusionWidth']
		#Set the up/down skins height to 10000 if we want a 100% filled object.
		# This gives better results then normal 100% infill as the sparse and up/down skin have some overlap.
		settings['downSkinCount'] = 10000
		settings['upSkinCount'] = 10000
	else:
		settings['sparseInfillLineDistance'] = int(100 * profile.calculateEdgeWidth() * 1000 / profile.getProfileSettingFloat('fill_density'))
	if profile.getProfileSetting('platform_adhesion') == 'Brim':
		settings['skirtDistance'] = 0
		settings['skirtLineCount'] = int(profile.getProfileSettingFloat('brim_line_count'))
	elif profile.getProfileSetting('platform_adhesion') == 'Raft':
		settings['skirtDistance'] = 0
		settings['skirtLineCount'] = 0
		settings['raftMargin'] = int(profile.getProfileSettingFloat('raft_margin') * 1000)
		settings['raftLineSpacing'] = int(profile.getProfileSettingFloat('raft_line_spacing') * 1000)
		settings['raftBaseThickness'] = int(profile.getProfileSettingFloat('raft_base_thickness') * 1000)
		settings['raftBaseLinewidth'] = int(profile.getProfileSettingFloat('raft_base_linewidth') * 1000)
		settings['raftInterfaceThickness'] = int(profile.getProfileSettingFloat('raft_interface_thickness') * 1000)
		settings['raftInterfaceLinewidth'] = int(profile.getProfileSettingFloat('raft_interface_linewidth') * 1000)
		settings['raftInterfaceLineSpacing'] = int(profile.getProfileSettingFloat('raft_interface_linewidth') * 1000 * 2.0)
		settings['raftAirGapLayer0'] = int(profile.getProfileSettingFloat('raft_airgap') * 1000 + profile.getProfileSettingFloat('raft_airgap_all') * 1000)
		settings['raftAirGap'] = int(profile.getProfileSettingFloat('raft_airgap_all') * 1000)
		settings['raftBaseSpeed'] = int(profile.getProfileSettingFloat('bottom_layer_speed'))
		settings['raftFanSpeed'] = 0
		settings['raftSurfaceThickness'] = int(profile.getProfileSettingFloat('raft_surface_thickness') * 1000)
		settings['raftSurfaceLinewidth'] = int(profile.getProfileSettingFloat('raft_surface_linewidth') * 1000)
		settings['raftSurfaceLineSpacing'] = int(profile.getProfileSettingFloat('raft_surface_linewidth') * 1000)
		settings['raftSurfaceLayers'] = int(profile.getProfileSettingFloat('raft_surface_layers'))
		settings['raftSurfaceSpeed'] = int(profile.getProfileSettingFloat('bottom_layer_speed'))
	else:
		settings['skirtDistance'] = int(profile.getProfileSettingFloat('skirt_gap') * 1000)
		settings['skirtLineCount'] = int(profile.getProfileSettingFloat('skirt_line_count'))
		settings['skirtMinLength'] = int(profile.getProfileSettingFloat('skirt_minimal_length') * 1000)

	if profile.getProfileSetting('fix_horrible_union_all_type_a') == 'True':
		settings['fixHorrible'] |= 0x01
	if profile.getProfileSetting('fix_horrible_union_all_type_b') == 'True':
		settings['fixHorrible'] |= 0x02
	if profile.getProfileSetting('fix_horrible_use_open_bits') == 'True':
		settings['fixHorrible'] |= 0x10
	if profile.getProfileSetting('fix_horrible_extensive_stitching') == 'True':
		settings['fixHorrible'] |= 0x04

	if settings['layerThickness'] <= 0:
		settings['layerThickness'] = 1000
	if profile.getMachineSetting('gcode_flavor') == 'UltiGCode':
		settings['gcodeFlavor'] = 1
	elif profile.getMachineSetting('gcode_flavor') == 'MakerBot':
		settings['gcodeFlavor'] = 2
	elif profile.getMachineSetting('gcode_flavor') == 'BFB':
		settings['gcodeFlavor'] = 3
	elif profile.getMachineSetting('gcode_flavor') == 'Mach3/LinuxCNC':
		settings['gcodeFlavor'] = 4
	elif profile.getMachineSetting('gcode_flavor') == 'RepRap (Volumetric)':
		settings['gcodeFlavor'] = 5
	if profile.getProfileSetting('spiralize') == 'True':
		settings['spiralizeMode'] = 1
	if profile.getProfileSetting('simple_mode') == 'True':
		settings['simpleMode'] = 1
	if profile.getProfileSetting('wipe_tower') == 'True' and extruderCount > 1:
		settings['wipeTowerSize'] = int(math.sqrt(profile.getProfileSettingFloat('wipe_tower_volume') * 1000 * 1000 * 1000 / settings['layerThickness']))
	if profile.getProfileSetting('ooze_shield') == 'True':
		settings['enableOozeShield'] = 1
	return settings

if __name__ == '__main__':
	main()

