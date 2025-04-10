"use client";
import { useState } from "react";

export default function Home() {
  const [fileMenuOpen, setFileMenuOpen] = useState(false);
  const [editMenuOpen, setEditMenuOpen] = useState(false);
  const [paymentDeadline, setPaymentDeadline] = useState("");
  const [formErrors, setFormErrors] = useState({});

  const toggleFileMenu = () => {
    setFileMenuOpen(!fileMenuOpen);
  };

  const toggleEditMenu = () => {
    setEditMenuOpen(!editMenuOpen);
  };

  const handleDateChange = (event) => {
    setPaymentDeadline(event.target.value);
  };

  const handleSubmit = async (event) => {
    event.preventDefault();
    const form = event.target;
    const errors = {};

    if (!form["払込番号"].value) errors["払込番号"] = "払込番号を入力してください";
    if (!form["金額"].value) errors["金額"] = "金額を入力してください";
    if (!form["お客様番号"].value) errors["お客様番号"] = "お客様番号を入力してください";
    if (!form["会社名"].value) errors["会社名"] = "会社名を入力してください";
    if (!form["お名前"].value) errors["お名前"] = "お名前を入力してください";
    if (!form["ご住所"].value) errors["ご住所"] = "ご住所を入力してください";
    if (!paymentDeadline) errors["払込期限"] = "払込期限を入力してください";

    setFormErrors(errors);

    if (Object.keys(errors).length === 0) {
      const formData = {
        払込番号: form["払込番号"].value,
        金額: form["金額"].value,
        お客様番号: form["お客様番号"].value,
        会社名: form["会社名"].value,
        お名前: form["お名前"].value,
        ご住所: form["ご住所"].value,
        払込期限: paymentDeadline,
      };

      try {
        const response = await fetch('/api/submit', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(formData),
        });

        if (response.ok) {
          alert("フォームが送信されました");
        } else {
          alert("送信に失敗しました");
        }
      } catch (error) {
        alert("送信中にエラーが発生しました");
      }
    }
  };

  return (
    <div style={{ backgroundColor: "white", height: "100vh" }}>
      <div style={{ display: "flex", padding: "10px", backgroundColor: "#f0f0f0", color: "black" }}>
        <div style={{ marginRight: "20px", position: "relative" }}>
          <div onClick={toggleFileMenu}>ファイル</div>
          {fileMenuOpen && (
            <div style={{ position: "absolute", top: "100%", left: 0, backgroundColor: "white", border: "1px solid #ccc" }}>
              <div style={{ padding: "5px 10px" }}>新規</div>
              <div style={{ padding: "5px 10px" }}>保存</div>
              <div style={{ padding: "5px 10px" }}>複製</div>
            </div>
          )}
        </div>
        <div style={{ marginRight: "20px", position: "relative" }}>
          <div onClick={toggleEditMenu}>編集</div>
          {editMenuOpen && (
            <div style={{ position: "absolute", top: "100%", left: 0, backgroundColor: "white", border: "1px solid #ccc" }}>
              <div style={{ padding: "5px 10px" }}>検索</div>
              <div style={{ padding: "5px 10px" }}>置換</div>
            </div>
          )}
        </div>
        <div>ツール</div>
      </div>
      <div style={{ display: "flex", height: "calc(100vh - 64px)" }}>
        <div style={{ width: "20%", backgroundColor: "#f0f0f0", padding: "10px" }}>
          {/* プロジェクトビュー */}
        </div>
        <div style={{ width: "80%", backgroundColor: "red", padding: "10px" }}>
          <form onSubmit={handleSubmit} style={{ backgroundColor: "white", padding: "10px", borderRadius: "5px", display: "flex", flexDirection: "column" }}>
            <input type="text" name="払込番号" placeholder="払込番号" required style={{ margin: "10px 0", padding: "10px", width: "100%" }} />
            {formErrors["払込番号"] && <div style={{ color: "red" }}>{formErrors["払込番号"]}</div>}
            <input type="text" name="金額" placeholder="金額" required style={{ margin: "10px 0", padding: "10px", width: "100%" }} />
            {formErrors["金額"] && <div style={{ color: "red" }}>{formErrors["金額"]}</div>}
            <input type="text" name="お客様番号" placeholder="お客様番号" required style={{ margin: "10px 0", padding: "10px", width: "100%" }} />
            {formErrors["お客様番号"] && <div style={{ color: "red" }}>{formErrors["お客様番号"]}</div>}
            <input type="text" name="会社名" placeholder="会社名" required style={{ margin: "10px 0", padding: "10px", width: "100%" }} />
            {formErrors["会社名"] && <div style={{ color: "red" }}>{formErrors["会社名"]}</div>}
            <input type="text" name="お名前" placeholder="お名前" required style={{ margin: "10px 0", padding: "10px", width: "100%" }} />
            {formErrors["お名前"] && <div style={{ color: "red" }}>{formErrors["お名前"]}</div>}
            <input type="text" name="ご住所" placeholder="ご住所" required style={{ margin: "10px 0", padding: "10px", width: "100%" }} />
            {formErrors["ご住所"] && <div style={{ color: "red" }}>{formErrors["ご住所"]}</div>}
            <input type="date" name="払込期限" placeholder="払込期限" value={paymentDeadline} onChange={handleDateChange} required style={{ margin: "10px 0", padding: "10px", width: "100%" }} />
            {formErrors["払込期限"] && <div style={{ color: "red" }}>{formErrors["払込期限"]}</div>}
            <button type="submit" style={{ margin: "20px 0", padding: "15px", fontSize: "1.25rem", backgroundColor: "#4CAF50", color: "white", border: "none", borderRadius: "5px", cursor: "pointer" }}>
              送信
            </button>
          </form>
        </div>
      </div>
    </div>
  );
}
