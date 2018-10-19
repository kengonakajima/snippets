"use strict";
exports.__esModule = true;
var SortType;
(function (SortType) {
    var TreasureBox;
    (function (TreasureBox) {
        TreasureBox[TreasureBox["SlotCreatedAsc"] = 0] = "SlotCreatedAsc";
        TreasureBox[TreasureBox["RemainAsc"] = 1] = "RemainAsc";
        TreasureBox[TreasureBox["RemainDesc"] = 2] = "RemainDesc"; // 解錠まで遅い順
    })(TreasureBox = SortType.TreasureBox || (SortType.TreasureBox = {}));
    ;
})(SortType = exports.SortType || (exports.SortType = {}));
;
exports.Sorting = {
    slot_created_asc: SortType.TreasureBox.SlotCreatedAsc,
    remain_desc: SortType.TreasureBox.RemainDesc,
    remain_asc: SortType.TreasureBox.RemainAsc
};
