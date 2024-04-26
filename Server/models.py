import uuid
from typing import Optional 
from pydantic import BaseModel, Field

class Entry (BaseModel):
    # id: str = Field(default_factory=lambda: str(uuid.uuid4()), alias="_id")
    temperature: float = Field(...)
    humidity: float = Field(...)
    index: float = Field(...)
    fan1: Optional[bool] = False
    fan2: Optional[bool] = False
    dangerLevel: Optional[int] = -1
    timestamp: Optional[str] = None
    class Config:
        populate_by_name = True


class EntryUpdate(BaseModel):
    temperature: Optional[float] = None
    humidity: Optional[float] = None
    index: Optional[float] = None
    fan1: Optional[bool] = None
    fan2: Optional[bool] = None
    dangerLevel: Optional[int] = None
    timestamp: Optional[str] = None
