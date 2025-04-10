import { useState } from 'react'
import './App.css'

function App() {
  const [formData, setFormData] = useState({
    documentNo: '',
    date: {
      year: '',
      month: '',
      day: ''
    },
    department: '',
    name: '',
    purpose: '',
    destination: '',
    period: {
      departure: {
        date: '',
        time: ''
      },
      return: {
        date: '',
        time: ''
      }
    },
    schedule: [
      { period: '', visitPlace: '', purpose: '', contact: '' }
    ],
    remarks: ''
  })

  const handleChange = (e, field, subField = null) => {
    if (subField) {
      setFormData({
        ...formData,
        [field]: {
          ...formData[field],
          [subField]: e.target.value
        }
      })
    } else {
      setFormData({
        ...formData,
        [field]: e.target.value
      })
    }
  }

  const handleScheduleChange = (index, field, value) => {
    const newSchedule = [...formData.schedule]
    newSchedule[index] = {
      ...newSchedule[index],
      [field]: value
    }
    setFormData({
      ...formData,
      schedule: newSchedule
    })
  }

  const addScheduleRow = () => {
    setFormData({
      ...formData,
      schedule: [
        ...formData.schedule,
        { period: '', visitPlace: '', purpose: '', contact: '' }
      ]
    })
  }

  return (
    <div className="business-trip-form">
      <h1 className="form-header">出張申請書</h1>
      
      <div className="header-row">
        <div className="document-no">
          <label>書類No.：</label>
          <input
            type="text"
            value={formData.documentNo}
            onChange={(e) => handleChange(e, 'documentNo')}
          />
        </div>
        <div>
          <label>平成</label>
          <input
            type="text"
            className="date-input"
            value={formData.date.year}
            onChange={(e) => handleChange(e, 'date', 'year')}
          />
          <label>年</label>
          <input
            type="text"
            className="date-input"
            value={formData.date.month}
            onChange={(e) => handleChange(e, 'date', 'month')}
          />
          <label>月</label>
          <input
            type="text"
            className="date-input"
            value={formData.date.day}
            onChange={(e) => handleChange(e, 'date', 'day')}
          />
          <label>日</label>
        </div>
      </div>

      <div className="form-row">
        <div className="form-group">
          <label>所属：</label>
          <input
            type="text"
            className="full-width"
            value={formData.department}
            onChange={(e) => handleChange(e, 'department')}
          />
        </div>
        <div className="form-group">
          <label>氏名：</label>
          <input
            type="text"
            className="full-width"
            value={formData.name}
            onChange={(e) => handleChange(e, 'name')}
          />
        </div>
      </div>

      <div className="form-row">
        <div className="form-group">
          <label>出張目的：</label>
          <input
            type="text"
            className="full-width"
            value={formData.purpose}
            onChange={(e) => handleChange(e, 'purpose')}
          />
        </div>
        <div className="form-group">
          <label>出張先：</label>
          <input
            type="text"
            className="full-width"
            value={formData.destination}
            onChange={(e) => handleChange(e, 'destination')}
          />
        </div>
      </div>

      <div className="form-row">
        <div className="form-group">
          <label>出発日：</label>
          <input
            type="date"
            value={formData.period.departure.date}
            onChange={(e) => handleChange(e, 'period.departure', 'date')}
          />
          <label style={{ marginLeft: '10px' }}>時刻：</label>
          <input
            type="time"
            value={formData.period.departure.time}
            onChange={(e) => handleChange(e, 'period.departure', 'time')}
          />
        </div>
        <div className="form-group">
          <label>帰着日：</label>
          <input
            type="date"
            value={formData.period.return.date}
            onChange={(e) => handleChange(e, 'period.return', 'date')}
          />
          <label style={{ marginLeft: '10px' }}>時刻：</label>
          <input
            type="time"
            value={formData.period.return.time}
            onChange={(e) => handleChange(e, 'period.return', 'time')}
          />
        </div>
      </div>

      <div className="form-row">
        <h3>スケジュール</h3>
        <table className="schedule-table">
          <thead>
            <tr>
              <th>期間</th>
              <th>訪問先</th>
              <th>用件</th>
              <th>連絡先</th>
            </tr>
          </thead>
          <tbody>
            {formData.schedule.map((row, index) => (
              <tr key={index}>
                <td>
                  <input
                    type="text"
                    value={row.period}
                    onChange={(e) => handleScheduleChange(index, 'period', e.target.value)}
                  />
                </td>
                <td>
                  <input
                    type="text"
                    value={row.visitPlace}
                    onChange={(e) => handleScheduleChange(index, 'visitPlace', e.target.value)}
                  />
                </td>
                <td>
                  <input
                    type="text"
                    value={row.purpose}
                    onChange={(e) => handleScheduleChange(index, 'purpose', e.target.value)}
                  />
                </td>
                <td>
                  <input
                    type="text"
                    value={row.contact}
                    onChange={(e) => handleScheduleChange(index, 'contact', e.target.value)}
                  />
                </td>
              </tr>
            ))}
          </tbody>
        </table>
        <button
          className="add-row-button"
          onClick={addScheduleRow}
        >
          行を追加
        </button>
      </div>

      <div className="remarks-section">
        <h3>備考・管理部門記入欄</h3>
        <textarea
          className="remarks-textarea"
          value={formData.remarks}
          onChange={(e) => handleChange(e, 'remarks')}
        />
      </div>
    </div>
  )
}

export default App
