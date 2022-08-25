#include <phi/test/test_macros.hpp>

#include "SetupImGui.hpp"
#include <DLXEmu/CodeEditor.hpp>
#include <DLXEmu/Emulator.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <phi/compiler_support/warning.hpp>

using namespace phi::literals;

PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")

static dlxemu::Emulator emulator;

TEST_CASE("crash-6ededd1eef55e21130e51a28a22b1275a0929cfd")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText("\n\n\n");
    editor.VerifyInternalState();

    editor.SetSelection(dlxemu::CodeEditor::Coordinates(0u, 1993065u),
                        dlxemu::CodeEditor::Coordinates(31u, 1761607680u));
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();

    editor.Undo(24u);
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("Crash-1c525126120b9931b78d5b724f6338435e211037")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.InsertText("\n");
    editor.VerifyInternalState();

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0u, 0u));
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();

    editor.SetSelectionStart(dlxemu::CodeEditor::Coordinates(0u, 30u));
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("Crash-a37f577acccdcbfa8bdc8f53a570e1c6385c13da")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.SetText("z`3!\n");
    editor.VerifyInternalState();

    editor.InsertText("\x1E");
    editor.VerifyInternalState();

    editor.MoveBottom(true);
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-2b9e8952b4d9676e2af93db7032ebca1dc2a9480")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.SetText("!");
    editor.VerifyInternalState();

    editor.SelectAll();
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-4161f8892d023e82832c668012743711e7e8c263")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.InsertText("\x02\x01");
    editor.VerifyInternalState();

    editor.MoveHome(true);
    editor.VerifyInternalState();

    editor.InsertText("\n");
    editor.VerifyInternalState();
}

TEST_CASE("crash-9caa85410b9d43f4c105d38ab169f0540d159648")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.InsertText("\x02\x01");
    editor.VerifyInternalState();

    editor.MoveHome(true);
    editor.VerifyInternalState();

    editor.InsertText("\n\n");
    editor.VerifyInternalState();
}

TEST_CASE("crash-0c744fcdb9b8193836417ce839daa3174ce89e16")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.SetText("U");
    editor.VerifyInternalState();

    editor.SetSelection(dlxemu::CodeEditor::Coordinates(7u, 1537u),
                        dlxemu::CodeEditor::Coordinates(738197504u, 30u));
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-4620fed3f283876c8534a78e77bbb319a9def029")
{
    dlxemu::CodeEditor editor(&emulator);

    std::vector<std::string> vec;
    vec.reserve(7);

    vec.emplace_back("");
    vec.emplace_back("");
    vec.emplace_back("");
    vec.emplace_back("");
    vec.emplace_back("");
    vec.emplace_back("");
    vec.emplace_back("\x1E");

    REQUIRE(vec.size() == 7);

    editor.SetTextLines(vec);
    editor.VerifyInternalState();

    editor.SetSelection(dlxemu::CodeEditor::Coordinates(0u, 30u),
                        dlxemu::CodeEditor::Coordinates(30u, 2883584u));
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-73ef47764c46d77f157ef9399720189dbbeaeee3")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.InsertText("(#8(\t");
    editor.VerifyInternalState();

    editor.MoveBottom(true);
    editor.VerifyInternalState();

    editor.Delete(); // Instead of cut
    editor.VerifyInternalState();
}

TEST_CASE("crash-ebbfccfff485022666d0448d53c7634d31f98c9a")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.InsertText("\t\x44\x4D");
    editor.VerifyInternalState();

    editor.MoveEnd(true);
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-aeb78eb087c7e15d3bc53666d21575ec7b73bd02")
{
    ImGuiContext* ctx = ImGui::CreateContext();
    REQUIRE(ctx);

    dlxemu::CodeEditor editor(&emulator);

    editor.InsertText("(#8(\x7F\t\x07");
    editor.VerifyInternalState();

    editor.Copy();
    editor.VerifyInternalState();

    editor.Paste();
    editor.VerifyInternalState();

    editor.Undo(638844961u);
    editor.VerifyInternalState();

    ImGui::DestroyContext(ctx);
}

TEST_CASE("crash-1bc6fd5daba7cdfcacbc166f238326b0b3ed7b1e")
{
    dlxemu::CodeEditor editor(&emulator);

    editor.InsertText("\tDM+");
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();

    editor.MoveBottom(true);
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-28853252177dc5b6be74f8247bde0d2a2b4f87b5")
{
    BeginImGui();

    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText("kA`\"#;#");
    editor.VerifyInternalState();

    CHECK(editor.GetText().size() == 7);
    CHECK(bool(editor.GetTotalLines() == 1u));

    editor.Render({0.0f, 0.0f}, true);
    editor.VerifyInternalState();

    EndImgui();
}

TEST_CASE("crash-c567e237f4822cff4cab65198f9ea3b393e6f92c")
{
    BeginImGui();

    dlxemu::CodeEditor editor{&emulator};

    editor.SetText(" ");
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.InsertText(":x;(");
    editor.VerifyInternalState();

    editor.Render({0.0f, 0.0f}, true);
    editor.VerifyInternalState();

    EndImgui();
}

TEST_CASE("crash-1e4a2c5c4b7bd8fe934c1eb3b5e0e98ed3474b72")
{
    BeginImGui();

    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter(0xFF, true);
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.InsertText("(m:M:x;");
    editor.VerifyInternalState();

    editor.Render({0.0f, 0.0f}, true);
    editor.VerifyInternalState();

    EndImgui();
}

TEST_CASE("Crash-b969d74f5fc10237a879950cd37541614ee459e4")
{
    BeginImGui();

    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText("A\tJ");
    editor.VerifyInternalState();

    editor.Render({0.0, 0.0}, true);
    editor.VerifyInternalState();

    editor.AddErrorMarker(0u, "");
    editor.VerifyInternalState();

    editor.AddErrorMarker(0u, "");
    editor.VerifyInternalState();

    editor.MoveRight(0u, true, true);
    editor.VerifyInternalState();

    editor.AddErrorMarker(0u, "");
    editor.VerifyInternalState();

    EndImgui();
}

TEST_CASE("crash-4e00b6223382d32d373d6d47d46d844a422c77a8")
{
    // Crash
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText(" \tJ");
        editor.VerifyInternalState();

        editor.Render({0.0, 0.0}, true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "       ");
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "      ");
        editor.VerifyInternalState();

        editor.MoveRight(0u, true, true);
        editor.VerifyInternalState();

        EndImgui();
    }

    // MoveLeft
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText(" \tJ");
        editor.VerifyInternalState();

        editor.Render({0.0, 0.0}, true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "       ");
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "      ");
        editor.VerifyInternalState();

        editor.MoveLeft(0u, true, true);
        editor.VerifyInternalState();

        EndImgui();
    }

    // MoveUp
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText(" \tJ");
        editor.VerifyInternalState();

        editor.Render({0.0, 0.0}, true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "       ");
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "      ");
        editor.VerifyInternalState();

        editor.MoveUp(0u, true);
        editor.VerifyInternalState();

        EndImgui();
    }

    // MoveDown
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText(" \tJ");
        editor.VerifyInternalState();

        editor.Render({0.0, 0.0}, true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "       ");
        editor.VerifyInternalState();

        editor.AddErrorMarker(538976288u, "      ");
        editor.VerifyInternalState();

        editor.MoveDown(0u, true);
        editor.VerifyInternalState();

        EndImgui();
    }
}

TEST_CASE("crash-57d1660b624969ec7a302988f2a6a3941dbcd5ef")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.ClearText();
    editor.VerifyInternalState();

    editor.Undo();
    editor.VerifyInternalState();
}

TEST_CASE("crash-b98edbf987ccf89dabea024daa1e7ef0d6d4617e")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.SelectAll();
    editor.VerifyInternalState();

    editor.ClearText();
    editor.VerifyInternalState();
}

TEST_CASE("crash-f84b5dc1d1204536f7f1c21183d53d46e01dbd55")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.MoveUp(4160756736u, true);
    editor.VerifyInternalState();

    (void)editor.GetCurrentLineText();
    editor.VerifyInternalState();
}

TEST_CASE("crash-f6475d7a88e545cb6d3a8530149b0dcd7619cbc2")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('\r', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-ff503611dc08748de0b5757421d910d75d775424")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText(" ");
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-de742aa77c3a1338ef0599275189c8347865a332")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText(" \"    ");
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.SelectAll();
    editor.VerifyInternalState();

    editor.EnterCharacter('\t', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-bba9ac4113ff3363a258b04abd8a8ef6d247d2bc")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('\0', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-6fe3797808cf5badc66c5647c9bcad8a3b789c44")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
                      "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t)(N");
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-f4fed5451dd3168ac0820727ea67a17373c491f3")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.SelectAll();
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-1201e7588fb65fb8bd0e21402378cd2eddcd46f0")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.SetText("iii#ii#");
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();

    dlxemu::CodeEditor::Coordinates coord;
    coord.m_Line   = uint32_t(-1140850622);
    coord.m_Column = uint32_t(-1644167102);
    editor.SetSelectionEnd(coord);
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-e96ca65627e2d85a40faf1ac32d475e9cad6f6f2")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText("\t");
    editor.VerifyInternalState();

    editor.Backspace();
    editor.VerifyInternalState();
}

TEST_CASE("crash-d62026e2b2402120e88c6b08fb4258629e587ee8")
{
    // Note: Requires DLXEMU_VERIFY_COLUMN
    dlxemu::CodeEditor editor{&emulator};

    editor.SetText("\tJJJJJ");
    editor.VerifyInternalState();

    editor.SetSelection({1577123585u, 1744830474u}, {1241710602u, 30u});
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-c8a3f2fd13c8b579fd54ef3ae4099e9d4514c47c")
{
    // Note: Requires DLXEMU_VERIFY_COLUMN
    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText("zjzU\t\t\t");
    editor.VerifyInternalState();

    editor.SetTabSize(0_u8);
    editor.VerifyInternalState();
}

TEST_CASE("crash-fb8cf6fc795f270cba6e88dcf700212057424304")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.SetText("\t\t\t");
    editor.VerifyInternalState();

    editor.SetCursorPosition({0u, 10u});
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();
}

TEST_CASE("crash-b814e3e7a67909918a8d94359459ae5f8f3a778e")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.SetText("\t");
    editor.VerifyInternalState();

    editor.SelectAll();
    editor.VerifyInternalState();

    editor.Delete();
    editor.VerifyInternalState();

    editor.SetTabSize(0_u8);
    editor.VerifyInternalState();

    editor.Undo();
    editor.VerifyInternalState();
}

TEST_CASE("crash-d71c35285a21907cbb3b49215fa07b1c47054b32")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter(0x4000, true);
    editor.VerifyInternalState();

    editor.EnterCharacter(0xFF00, true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-f8d09e403b9ec44efacf944db275e4a4470855b3")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "\n");
    CHECK(bool(editor.GetTotalLines() == 2u));
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 0u});
    CHECK_FALSE(editor.HasSelection());
    CHECK(editor.CanUndo());

    editor.MoveUp(538976288u, true);
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "\n");
    CHECK(bool(editor.GetTotalLines() == 2u));
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1u, 0u});
    CHECK(editor.GetSelectedText() == "\n");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

    // NOTE: Should be NOP since its not a valid char
    editor.EnterCharacter(0xDF20, true);
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "\n");
    CHECK(bool(editor.GetTotalLines() == 2u));
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1u, 0u});
    CHECK(editor.GetSelectedText() == "\n");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

    editor.Undo();
    editor.VerifyInternalState();

    CHECK(editor.GetText().empty());
    CHECK(bool(editor.GetTotalLines() == 1u));
    CHECK_FALSE(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectedText().empty());
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});
}

TEST_CASE("crash-a984a771c4227705ec8b87dedf02531b2b87a0ef")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter(0xD800, true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-b3eb38a122a8b517777553ac35921fd8878c964e")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.MoveLeft(1024u, true, true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-f2b41ee47674a8c822a660b7b932eccef3a2875e")
{
    // NOTE: Requires DLXEMU_VERIFY_UNDO_REDO
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter(0x2020, true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-d439bbd4f3241cfe0097964d228de8f06536d01c")
{
    dlxemu::CodeEditor editor{&emulator};

    (void)editor.GetText();
    editor.VerifyInternalState();
}

TEST_CASE("crash-a4ac408fb9d6def070ad3a76312ca092863048e5")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.SetText(std::string(0xdd, 1u));
    editor.VerifyInternalState();

    volatile std::string res = editor.GetText();
    (void)res;
    editor.VerifyInternalState();
}

TEST_CASE("crash-d119db6a8c3f6f94864cf29ff9b426e53bf315c7")
{
    static constexpr const phi::usize runs = 10u;
    for (phi::usize index{0u}; index < runs; ++index)
    {
        BeginImGui();

        dlxemu::Emulator   emu;
        dlxemu::CodeEditor editor{&emu};

        editor.EnterCharacter(0xFE0B, true);
        editor.VerifyInternalState();

        editor.Render(ImVec2{0.0f, 8795850276864.000000f}, true);
        editor.VerifyInternalState();

        EndImgui();
    }
}

TEST_CASE("crash-dd60a4533d7fd2e1f6a974f2d7bb139335aa739c")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.AddErrorMarker(0xFFFFFFFF, "    ");
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-f37f2a81d1309cf2fc5d8b354e09fa22076a2826")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.AddErrorMarker(0u, "  ");
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.Backspace();
    editor.VerifyInternalState();
}

TEST_CASE("crash-ac303d9db5dd084def01d83275fb76088b3c0651")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();

    editor.MoveDown(0xFFFFFFFF, true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-a2facddc6dcca65b1ed29b8b2db9860e5d85de5e")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.SetBreakpoints({0xFFFFFFFF});
    editor.VerifyInternalState();

    editor.EnterCharacter('\n', true);
    editor.VerifyInternalState();
}

TEST_CASE("crash-dba6d073dbbd3540a269e29713a6fdd2ec07810a")
{
    // Original
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(1u, "");
        editor.VerifyInternalState();

        editor.Undo();
        editor.VerifyInternalState();
    }

    // Same but with break point
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.AddBreakpoint(1u);
        editor.VerifyInternalState();

        editor.Undo();
        editor.VerifyInternalState();
    }
}

TEST_CASE("crash-fb51886e58a0f6657347eadf6e435a5253946875")
{
    // Original
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.SetBreakpoints({2u});
        editor.VerifyInternalState();

        editor.ClearText();
        editor.VerifyInternalState();
    }

    // Same but with error markers
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(2u, "");
        editor.VerifyInternalState();

        editor.ClearText();
        editor.VerifyInternalState();
    }
}

TEST_CASE("crash-749abb6be89c8e168059ab62c631ccb0a3c12f07")
{
    BeginImGui();

    dlxemu::CodeEditor editor{&emulator};

    ImGui::GetIO().AddInputCharacterUTF16(57339);
    editor.VerifyInternalState();

    editor.Render({0.000000, -605108545849894421659648.000000});
    editor.VerifyInternalState();

    EndImgui();
}
