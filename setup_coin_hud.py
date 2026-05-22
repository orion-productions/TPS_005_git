"""
setup_coin_hud.py
Run from UE Editor Python console:
    py "D:/GameDev/Unreal Projects/TPS_005_git/setup_coin_hud.py"

Builds WBP_CoinHUD: Canvas Panel root + Text Block anchored bottom-left.
"""
import unreal

WIDGET_PATH = '/Game/ThirdPerson/Blueprints/WBP_CoinHUD'

wb   = unreal.load_asset(WIDGET_PATH)
tree = unreal.load_object(None, WIDGET_PATH + '.WBP_CoinHUD:WidgetTree')

# --- canvas panel -----------------------------------------------------------
canvas = unreal.load_object(None, WIDGET_PATH + '.WBP_CoinHUD:WidgetTree.CanvasPanel_Root')
if not canvas:
    canvas = unreal.new_object(unreal.CanvasPanel, tree, 'CanvasPanel_Root')
    unreal.log('[HUD] created CanvasPanel_Root')
else:
    unreal.log('[HUD] CanvasPanel_Root already exists')

# Set as root via the UWidgetTree C++ API through a transact
with unreal.ScopedEditorTransaction('HUD SetRootWidget') as trans:
    tree.modify()
    # Access via internal property name (cpp: RootWidget, serialized as 'RootWidget')
    try:
        tree.set_editor_property('RootWidget', canvas)
        unreal.log('[HUD] RootWidget set via transaction')
    except Exception as e:
        unreal.log_warning(f'[HUD] set_editor_property failed: {e}')
        # Fallback: use K2 helper
        try:
            unreal.WidgetBlueprintLibrary.set_root_widget(tree, canvas)
            unreal.log('[HUD] RootWidget set via WidgetBlueprintLibrary')
        except Exception as e2:
            unreal.log_warning(f'[HUD] WidgetBlueprintLibrary fallback failed: {e2}')

# --- text block -------------------------------------------------------------
txt_path = WIDGET_PATH + '.WBP_CoinHUD:WidgetTree.Text_CoinCount'
txt = unreal.load_object(None, txt_path)
if not txt:
    txt = unreal.new_object(unreal.TextBlock, tree, 'Text_CoinCount')
    unreal.log('[HUD] created Text_CoinCount')
else:
    unreal.log('[HUD] Text_CoinCount already exists')

fs = txt.get_editor_property('Font')
fs.set_editor_property('Size', 28)
txt.set_editor_property('Font', fs)
txt.set_editor_property('ColorAndOpacity',
    unreal.SlateColor(unreal.LinearColor(1.0, 1.0, 0.2, 1.0)))  # yellow
txt.set_editor_property('Text', unreal.Text.cast(unreal.Text, 'COINS: 0'))

# attach to canvas
slot = canvas.add_child_to_canvas(txt)
slot.set_editor_property(
    'Anchors',
    unreal.Anchors(minimum=unreal.Vector2D(0, 1),
                   maximum=unreal.Vector2D(0, 1)))
slot.set_editor_property('Alignment', unreal.Vector2D(0.0, 1.0))
slot.set_editor_property('Position',  unreal.Vector2D(20.0, -20.0))
slot.set_editor_property('Size',      unreal.Vector2D(300.0, 50.0))
unreal.log('[HUD] Text_CoinCount slotted bottom-left')

# --- compile & save ---------------------------------------------------------
unreal.BlueprintEditorLibrary.compile_blueprint(wb)
unreal.EditorAssetLibrary.save_asset(WIDGET_PATH)
unreal.log('[HUD] Done. WBP_CoinHUD compiled and saved.')
unreal.log('[HUD] NOTE: If RootWidget is still unset, open WBP_CoinHUD in the')
unreal.log('[HUD]       Widget Designer and the Canvas+Text will be visible there.')
